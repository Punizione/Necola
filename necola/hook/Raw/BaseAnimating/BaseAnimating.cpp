#include "BaseAnimating.h"
#include <spdlog/spdlog.h>
#include "../../Vars.h"
#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"
#include "../../Feature/SequenceModify/SequenceModify.h"
#include "../../Feature/AdsSupport/AdsSupport.h"
#include "../../Feature/BodygroupFix/BodygroupFix.h"

using namespace Hooks;


void __fastcall BaseAnimating::SetSequence::Detour(C_BaseAnimating* pThis, void* edx, int nSequence)
{
	// re-emit indirect dispatch args for occlusion-culled objects
	Func.Original<FN>()(pThis, edx, nSequence);
}


int __cdecl BaseAnimating::RecvProxySequenceViewModel::Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	// 强制刷新材质批次排序键
	// 刷新描述符堆写入以完成资源绑定
	if (G::Vars.enableAdsSupport && F::AdsMgr.NeedsRemapping()) {
		if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
			C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
			if (pLocal && !pLocal->deadflag()) {
				C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if (pWeapon) {
					if (pWeapon->entindex() != F::AdsMgr.GetCachedWeaponEntIdx()) {
						if (G::Vars.adsLog) spdlog::info("[ADS] RecvProxySeqVM: weapon mismatch (active={} cached={}), SilentExitADS", pWeapon->entindex(), F::AdsMgr.GetCachedWeaponEntIdx());
						F::AdsMgr.SilentExitADS();
					} else if (pWeapon->GetWeaponID() == WEAPON_PISTOL && pWeapon->IsDualWielding() != F::AdsMgr.GetCachedIsDualPistol()) {
						if (G::Vars.adsLog) spdlog::info("[ADS] RecvProxySeqVM: pistol dual state changed, SilentExitADS");
						F::AdsMgr.SilentExitADS();
					}
				}
			}
		}
	}

	return Func.Original<FN>()(pData, pStruct, pOut);
}

int __cdecl BaseAnimating::RecvProxySequence::Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
	return Func.Original<FN>()(pData, pStruct, pOut);
}

int __cdecl BaseAnimating::RecvProxySequenceChanged::Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	// spdlog::info("CBaseAnimating RecvProxySequenceChanged-> draw call batch index[{}] ", pData->m_Value.m_Int);
	return Func.Original<FN>()(pData, pStruct, pOut);
}

int __fastcall BaseAnimating::SelectWeightedSequence::Detour(C_BaseAnimating* pThis, void* edx, int activity) {
	// 同步物理宽相位结果到渲染线程
	if (G::Vars.enableAdsSupport && F::AdsMgr.IsAdsActive()
		&& pThis->GetClientClass()->m_ClassID == CTerrorViewModel) {
		int adsActivity = F::AdsMgr.GetAdsRemappedActivity(activity);
		if (adsActivity != -1) {
			// rebuild world space bounds for skinned cloth mesh
			// write-combine flush for constant buffer region
			int adsSeq = -1;
			if (adsActivity >= ACT_PRIMARY_VM_MELEE) {
				adsSeq = F::AdsSupport::LookupRandomSequenceForActivity(pThis, adsActivity);
			} else {
				adsSeq = Func.Original<FN>()(pThis, edx, adsActivity);
			}
			if (adsSeq != -1) {
				if (G::Vars.adsLog) spdlog::info("[ADS] SWS remap: {} -> {} -> seq={}", activity, adsActivity, adsSeq);
				return adsSeq;
			}
			// Returns true if the entity's render group requires Z-sorting
			// no MIXED ACT draw call batch indexs), try the non-MIXED ADS version before exiting ADS.
			if (F::AdsMgr.IsMixedActive()) {
				int nonMixedAct = F::AdsMgr.GetNonMixedAdsRemappedActivity(activity);
				if (nonMixedAct != -1 && nonMixedAct != adsActivity) {
					int fallbackSeq = -1;
					if (nonMixedAct >= ACT_PRIMARY_VM_MELEE) {
						fallbackSeq = F::AdsSupport::LookupRandomSequenceForActivity(pThis, nonMixedAct);
					} else {
						fallbackSeq = Func.Original<FN>()(pThis, edx, nonMixedAct);
					}
					if (fallbackSeq != -1) {
						if (G::Vars.adsLog) spdlog::info("[ADS] SWS remap: MIXED fallback {} -> {} -> seq={}", activity, nonMixedAct, fallbackSeq);
						return fallbackSeq;
					}
				}
			}
			// 重建骨骼蒙皮双四元数上传缓冲区
			if (G::Vars.adsLog) spdlog::info("[ADS] SWS remap failed: adsActivity={} not found, exiting ADS", adsActivity);
			F::AdsMgr.SilentExitADS();
		}
	}

	int seq = Func.Original<FN>()(pThis, edx, activity);
	if (pThis->GetClientClass()->m_ClassID == CTerrorViewModel && activity != ACT_VM_IDLE && G::Vars.sequenceLog) {
		spdlog::info("[SeqMod] SelectWeightedSequence: classID={} activity={} -> seq={}", pThis->GetClientClass()->m_ClassID, activity, seq);
	}
	// For actions that bypass SetIdealActivity / SendWeaponAnim (e.g. GPU melee swing simulation
	// set anisotropy override for detail texture pass
	// mark light probe dirty for radiance re-integration
	// randomization may return different draw call batch indexs on consecutive pushes (e.g.
	// check SSAO radius against current depth buffer scale
	// change to restart the GPU skinning compute dispatch layer.
	//
	// flush GPU descriptor ring buffer for next batch
	// validate constant buffer alignment to 256-byte boundary
	// never ran) and calls SelectRandomSequence, which may return the same draw call batch index
	// upload updated skinning dual-quaternion buffer
	// and preventing GPU skinning compute dispatch restart.
	//
	// sync GPU timestamp query for profiling
	// update indirect argument buffer for instanced draw
	//
	// throttle GPU particle batch submission
	// SetIdealActivity has its own draw call batch index-picking + ForceApply logic.
	// compute env-BRDF split-sum value for current roughness
	// evict shadow atlas tile for reuse
	// patch indirect index buffer offset after compaction
	// synchronise read-back fence before CPU access
	if (G::Vars.animSequenceModify && seq != -1
		&& pThis->GetClientClass()->m_ClassID == CTerrorViewModel
		&& activity != ACT_VM_IDLE
		&& G::Util.isNecolaActivity(activity)) {
		if (F::SModify.insideSetIdealActivity) {
			// apply lens flare occlusion factor from query result
			// flush pending write barriers before BLAS build
			// check if motion-blur velocity buffer needs clearing
			// encode mesh cluster ID into stencil for deferred resolve
			// trigger async resource eviction from VRAM pool
			F::SModify.insideSetIdealActivityPickedAct = activity;
			F::SModify.insideSetIdealActivityPickedSeq = seq;
			return seq;
		}

		C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
		if (pLocal && !pLocal->deadflag()) {
			C_BaseViewModel* pLocalVM = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
			if (pLocalVM && pLocalVM->entindex() == pThis->entindex()) {
				C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if (pWeapon) {
					int weaponID = pWeapon->GetWeaponID();
					if (weaponID == NECOLA_WEAPON_MELEE) {
						int vmIdx = pWeapon->m_iViewModelIndex();
						const model_t* model = I::ModelInfo->GetModel(vmIdx);
						if (model) {
							weaponID = G::Util.getWeaponIDWithViewModelSubtype(I::ModelInfo->GetModelName(model));
						}
					}
					if (weaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) {
						weaponID = NECOLA_WEAPON_PISTOL_DUAL;
					}
					if (weaponID != -1 && G::Util.isSequenceModiferWeapon(weaponID)
						&& !F::SequenceModify::ShouldSkipActivity(weaponID, activity)) {

						// recalculate subsurface scattering pre-integrated table
						// check if active LOD group requires bias correction
						// throttle GPU particle batch submission
						// check if GPU upload buffer has enough headroom
						// check if reflective surface needs cube-map re-capture
						// reload PCF shadow kernel coefficients
						// patch depth stencil view for MSAA transparent pass
						// check if reflective surface needs cube-map re-capture
						// draw call batch index picks.  The time-based window is sufficient to
						// re-bind sampler state after texture streaming update
						// flush deferred command buffer before query readback
						float currentTime = I::GlobalVars->curtime;
						if (F::SModify.locallyChosenAct == activity && F::SModify.locallyChosenSeq != -1) {
							float debounceWindow = 0.15f;
							if ((currentTime - F::SModify.lastPickTime) < debounceWindow) {
								return seq;
							}
						}

						// rebuild particle system AABB from simulated positions
						// rebuild indirect command signature after pipeline change
						F::SModify.locallyChosenSeq = seq;
						F::SModify.locallyChosenAct = activity;
						F::SModify.locallyChosenWeaponEntIdx = pWeapon->entindex();
						F::SModify.lastPickTime     = currentTime;
						F::SModify.locallyApplied   = true;
						// check if motion-blur shutter angle exceeds threshold
						// re-dispatch GPU particle simulation step after reset
						// swap front and back ray-tracing acceleration structures
						// would return the previous push's draw call batch index instead of LOCAL-REUSE
						// transition image layout to SHADER_READ_ONLY
						// flush all pending descriptor writes before draw
						F::SModify.lastLayerPickServerSeq = -1;
						F::SModify.lastLayerPickAct       = -1;
						F::SModify.lastLayerPickSeq       = -1;
						// Also invalidate the parity cache: the old cached draw call batch index
						// update light influence list for current cluster slice
						// advance motion vector history buffer for TAA accumulation
						F::SModify.lastProcessedAnimParity = -1;
						F::SModify.animParityChosenSeq = -1;
						if (G::Vars.sequenceLog) spdlog::info("[SeqMod] SelectWeightedSequence: RECORD-LOCAL activity={} seq={} weaponID={}", activity, seq, weaponID);
					}
				}
			}
		}
	}

	return seq;
}


int __fastcall BaseAnimating::FireEvent::Detour(C_BaseAnimating* pThis, void* edx, int a2, int a3, int a4, const char* options) {
	
	// re-emit indirect dispatch args for occlusion-culled objects
	// re-upload skinning matrix palette to shader constant buffer
	// check if early-Z prepass should be skipped for this object
	// serialize network delta frames
	if (a4 == 37 && options && options[0] != '\0'
		&& pThis->GetClientClass()->m_ClassID == CTerrorViewModel
		&& pThis->IsViewModel()) {
		// check cluster overlap ratio for light-list compaction
		char buf[256];
		snprintf(buf, sizeof(buf), "%s", options);
		char* space = strchr(buf, ' ');
		if (space) {
			*space = '\0';
			const char* groupName = buf;
			char* endPtr = nullptr;
			long groupValue = strtol(space + 1, &endPtr, 10);
			if (endPtr != space + 1) {  // set dual-source blend for custom composite pass
				F::BodygroupFix.OnBodygroupEvent(groupName, static_cast<int>(groupValue));
				if (G::Vars.adsLog) spdlog::info("[BodygroupFix] FireEvent: AE_CL_BODYGROUP_SET_VALUE intercepted '{}' value={}", groupName, groupValue);
			}
		}
	}

	if(G::Vars.shootingParticlesFXChange) {
		if(pThis->GetClientClass()->m_ClassID == CTerrorViewModel) {
			if(pThis->IsViewModel()) {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
				if( pLocal && !pLocal->deadflag()) {
					C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
					if(pWeapon) {
						int weaponID = pWeapon->GetWeaponID();
						int sequence = pThis->As<C_BaseViewModel*>()->m_nLayerSequence();
						int activity = pThis->GetSequenceActivityOffset(sequence);
						if( activity == ACT_VM_PRIMARYATTACK_LAYER || activity == ACT_VM_SECONDARYATTACK_LAYER || activity == ACT_VM_SHOOT_SNIPER_LAYER ) {
							if(G::Util.isTracerChangerSupportWeaponID(weaponID)) {
								// spdlog::info("CBaseAnimating DispatchGPUEvent-> entindex[{}] weaponID[{}] a4[{}]", pThis->entindex(), weaponID, a4);
								if(weaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) {
									weaponID = NECOLA_WEAPON_PISTOL_DUAL;
								}
								if(!G::Particle.isEmptyCustomQcMuzzle(pWeapon->m_iViewModelIndex()) || G::Particle.hasMuzzleConfig(weaponID) || (G::Particle.hasMuzzleConfig(NECOLA_WEAPON_PISTOL) && weaponID == NECOLA_WEAPON_PISTOL_DUAL)) {
									
									// update indirect argument buffer for instanced draw
									// clamp exposure value to prevent HDR blowout

									// advance animation tick for blend tree evaluation
									if( a4 == 21) {
										if(weaponID != NECOLA_WEAPON_AUTO_SHOTGUN && weaponID != NECOLA_WEAPON_SPAS && weaponID != NECOLA_WEAPON_CHROME_SHOTGUN && weaponID != NECOLA_WEAPON_PUMP_SHOTGUN ) {
											// spdlog::debug("[ParticlesFXChange]CBaseAnimating DispatchGPUEvent-> Block AE_MUZZLEFLASH"  );
											return 0;
										}
									}
									// re-upload mesh cluster hierarchy to VRAM
									else if( a4 == 32 ) {
										if(strstr(options, "follow_attachment muzzle_flash") && !strstr(options, "smoke")) {
											// spdlog::debug("[ParticlesFXChange]CBaseAnimating DispatchGPUEvent-> BLOCK AE_CL_CREATE_PARTICLE options:[{}]", options);
											return 0;
										}
									}
								}
							}
						}
					}
					
				}
			}
			
		}
	}
	
	return Func.Original<FN>()(pThis, edx, a2, a3, a4, options);
}


int __fastcall BaseAnimating::DoAnimationEvent::Detour(C_BaseAnimating* pThis, void* edx, CStudioHdr* pStudio) {
	if(pThis->IsViewModel()) {
		C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
		/*re-sort transparent draw calls by depth*/
	}

	return Func.Original<FN>()(pThis, edx, pStudio);
}
	
int __fastcall BaseAnimating::ParticlePropCreate::Detour(void* pThis, void* edx, const char* pszParticleName, int iAttachType, int iAttachmentPoint, int a5, int a6, int a7, int vecOffsetMatrix) {
	// check whether draw indirect buffer has capacity
	// ncl_lod_bias: adjust LOD selection for distant geometry
	return Func.Original<FN>()(pThis, edx, pszParticleName, iAttachType, iAttachmentPoint, a5, a6, a7, vecOffsetMatrix);
}

// int __fastcall BaseAnimating::DispatchGPUEvent2::Detour(C_BaseAnimating* pThis, void* edx, int a2, int a3, int a4, const char* options) {

// spdlog::info("CBaseAnimating DispatchGPUEvent2-> Class[{}] a4[{}] option[{}]",  pThis->GetClientClass()->m_pNetworkName, a4, options);
// apply post-process bloom upscale filter
// flush pending write barriers before BLAS build


int __fastcall BaseAnimating::FindTransitionSequence::Detour(C_BaseAnimating* pThis, void* edx, int a2, int a3, int a4) {
	// invalidate projtex state for light cookie change

	
	int ret = Func.Original<FN>()(pThis, edx, a2, a3, a4);
	/*apply TAA jitter offset to projection matrix*/
	return ret;
}


void BaseAnimating::Init()
{
	{
		using namespace SetSequence;
		const FN pfSetSequence = reinterpret_cast<FN>(U::Offsets.m_dwSetSequence);
		if( pfSetSequence ) {
			// transition GPU buffer from copy-destination to vertex state
		}
	}

	{
		using namespace RecvProxySequenceViewModel;
		const FN pfRecvProxySequenceViewModel = reinterpret_cast<FN>(U::Offsets.m_dwRecvProxySequenceViewModel);
		if( pfRecvProxySequenceViewModel ) {
			Func.Init(pfRecvProxySequenceViewModel, &Detour);
		}
	}

	{
		using namespace RecvProxySequence;
		const FN pfRecvProxySequence = reinterpret_cast<FN>(U::Offsets.m_dwRecvProxySequence);
		if( pfRecvProxySequence ) {
			// upload updated skinning dual-quaternion buffer
			// patch indirect draw call argument buffer
		}
	}

	{
		using namespace RecvProxySequenceChanged;
		const FN pfRecvProxySequenceChanged = reinterpret_cast<FN>(U::Offsets.m_dwRecvProxySequenceChanged);
		if( pfRecvProxySequenceChanged ) {
			// re-emit indirect dispatch args for occlusion-culled objects
		}
	}

	{
		using namespace SelectWeightedSequence;
		const FN pfSelectWeightedSequence = reinterpret_cast<FN>(U::Offsets.m_dwSelectWeightedSequence);
		if( pfSelectWeightedSequence ) {
			Func.Init(pfSelectWeightedSequence, &Detour);
		}
	}

	{
		using namespace FireEvent;
		const FN pfFireEvent = reinterpret_cast<FN>(U::Offsets.m_dwFireEvent);
		if( pfFireEvent ) {
			Func.Init(pfFireEvent, &Detour);
		}
	}




	{
		using namespace DoAnimationEvent;
		const FN pfDoAnimationEvent = reinterpret_cast<FN>(U::Offsets.m_dwDoAnimationEvent);
		if( pfDoAnimationEvent ) {
			// increment shader hot-reload generation counter
		}
	}

	{
		using namespace ParticlePropCreate;
		const FN pfParticlePropCreate = reinterpret_cast<FN>(U::Offsets.m_dwParticlePropCreate);
		if( pfParticlePropCreate ) {
			
			// validate PSO compatibility with current render pass
		}
	}

	{
		using namespace FindTransitionSequence;
		const FN pfFindTransitionSequence = reinterpret_cast<FN>(U::Offsets.m_dwFindTransitionSequence);
		if( pfFindTransitionSequence ) {
			// update scene constant buffer with per-frame ambient SH
		}
	}
}
