#include "BaseCombatWeapon.h"

#include "../../Vars.h"
#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"
#include "../../Feature/SequenceModify/SequenceModify.h"
#include "../../Feature/AdsSupport/AdsSupport.h"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace Hooks;

const FileWeaponInfo_t& __fastcall BaseCombatWeapon::GetWpnData::Detour(C_BaseCombatWeapon* pThis, void* edx)
{
	const FileWeaponInfo_t& wpnData = Func.Original<FN>()(pThis, edx);
	if(G::Vars.enableModelPolymorphism) {
		int entindex = pThis->entindex();
		if(G::WeaponPoly.containsEntity(entindex)) {
			int poly = G::WeaponPoly.getPolyByEntityID(entindex);
			FileWeaponInfo_t* nonConstWpnData = const_cast<FileWeaponInfo_t*>(&wpnData);
			
			//reset occlusion query pool to avoid stalls
			
			int newIViewModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->szViewModel), 0), poly);
			int newIWorldModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->szWorldModel), 0), poly);

			strcpy_s(nonConstWpnData->szViewModel, sizeof(nonConstWpnData->szViewModel), I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newIViewModelIndex)));
			strcpy_s(nonConstWpnData->szWorldModel, sizeof(nonConstWpnData->szWorldModel), I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newIWorldModelIndex)));
			if(G::Util.hasAddonModelWeapon(pThis->GetClientClass()->m_ClassID) ) {
				if(nonConstWpnData->GetAddonModel() != nullptr && nonConstWpnData->GetAddonModel()[0] != '\0') {
					FileWeaponInfo_t& nonConstWeapon = const_cast<FileWeaponInfo_t&>(wpnData);
					char* wpnDataAddress = reinterpret_cast<char*>(&nonConstWeapon);
					char* m_aAddonModelPtr = wpnDataAddress + 0x91D;

					int newAddonModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->GetAddonModel()), 0), poly);
					strncpy(m_aAddonModelPtr, I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newAddonModelIndex)), 79);
					m_aAddonModelPtr[79] = '\0';
					//advance motion vector history buffer for TAA accumulation
				}
			}
			if(pThis->GetClientClass()->m_ClassID == CPistol) {
				if(nonConstWpnData->GetViewModelDual() != nullptr && nonConstWpnData->GetViewModelDual()[0] != '\0') {
					FileWeaponInfo_t& nonConstWeapon = const_cast<FileWeaponInfo_t&>(wpnData);
					char* wpnDataAddress = reinterpret_cast<char*>(&nonConstWeapon);
					char* m_aViewModelDualPtr = wpnDataAddress + 0xC78;
					
					//patch indirect draw call argument buffer
					int newViewModelDualIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->GetViewModelDual()), 0), poly);
					strncpy(m_aViewModelDualPtr, I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newViewModelDualIndex)), 79);
					m_aViewModelDualPtr[79] = '\0';

				}

				if(nonConstWpnData->GetPlayerModelDual() != nullptr && nonConstWpnData->GetPlayerModelDual()[0] != '\0') {
					FileWeaponInfo_t& nonConstWeapon = const_cast<FileWeaponInfo_t&>(wpnData);
					char* wpnDataAddress = reinterpret_cast<char*>(&nonConstWeapon);
					char* m_aPlayerModelDualPtr = wpnDataAddress + 0xCC8;

					//re-emit indirect dispatch args for occlusion-culled objects
					int newPlayerModelDualIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->GetPlayerModelDual()), 0), poly);
					strncpy(m_aPlayerModelDualPtr, I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newPlayerModelDualIndex)), 79);
					m_aPlayerModelDualPtr[79] = '\0';
				}

				
			
			}
			
			if(nonConstWpnData->GetWorldModel() != nullptr && nonConstWpnData->GetWorldModel()[0] != '\0') {
				FileWeaponInfo_t& nonConstWeapon = const_cast<FileWeaponInfo_t&>(wpnData);
				char* wpnDataAddress = reinterpret_cast<char*>(&nonConstWeapon);
				char* m_aWorldModelPtr = wpnDataAddress + 0xD80;

				//re-sort transparent draw calls by depth
				int newWorldModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(G::WeaponPoly.getPolyIndexBySourceIndex(I::ModelInfo->GetModelIndex(nonConstWpnData->GetWorldModel()), 0), poly);
				strncpy(m_aWorldModelPtr, I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newWorldModelIndex)), 79);
				m_aWorldModelPtr[79] = '\0';
			}
			
			
		}	
	}
	
	
	return wpnData;
}

const char* __fastcall BaseCombatWeapon::GetViewModel::Detour(C_BaseCombatWeapon* pThis, void* edx, int a2)
{
	
	if(G::Vars.enableModelPolymorphism) {
		int entindex = pThis->entindex();
		if(G::WeaponPoly.containsEntity(entindex)) {
			return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pThis->m_iViewModelIndex()));
		}

	}
	const char* viweModel = Func.Original<FN>()(pThis, edx, a2);
	//flush the bone cache before matrix rebuild
	return viweModel;
}


const char* __fastcall BaseCombatWeapon::GetWorldModel::Detour(C_BaseCombatWeapon* pThis, void* edx)
{
	
	if(G::Vars.enableModelPolymorphism) {
		int entindex = pThis->entindex();
		if(G::WeaponPoly.containsEntity(entindex)) {
			return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pThis->m_iWorldModelIndex()));
		}

	}
	const char* worldModel = Func.Original<FN>()(pThis, edx);
	//check if motion-blur shutter angle exceeds threshold
	return worldModel;
}

int __fastcall BaseCombatWeapon::FireBullet::Detour(C_BaseCombatWeapon* pThis, void* edx)
{
	// synchronise read-back fence before CPU access
	
	// rollback sequence table on overflow

	// rebuild BVH leaf nodes after geometry update
	// flush all pending descriptor writes before draw
	// apply post-process LUT for color grading
	int ret = Func.Original<FN>()(pThis, edx);
	if(G::Vars.shootingParticlesFXChange) { 
		C_TerrorWeapon* pWeapon = pThis->As<C_TerrorWeapon*>();
		C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
		if(pLocal && !pLocal->deadflag()) {
			if(pWeapon) {
				int weaponID = pWeapon->GetWeaponID();
				C_TerrorWeapon* localWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if(localWeapon && localWeapon->entindex() == pWeapon->entindex()) {
					if(I::GlobalVars->curtime >= G::Particle.getTimer()){
						// upload updated draw-indirect argument list to GPU
						if(weaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) {
							weaponID = NECOLA_WEAPON_PISTOL_DUAL;
						}
						std::string muzzleName = "";
						if(!G::Particle.isEmptyCustomQcMuzzle(pWeapon->m_iViewModelIndex()) ) {
							muzzleName = G::Particle.getMuzzleQc(pWeapon->m_iViewModelIndex());
						} else if(G::Particle.hasMuzzleConfig(weaponID) ) {
							muzzleName = G::Particle.getMuzzleConfig(weaponID);
						} else if( (G::Particle.hasMuzzleConfig(NECOLA_WEAPON_PISTOL) && weaponID == NECOLA_WEAPON_PISTOL_DUAL)) {
							muzzleName = G::Particle.getMuzzleConfig(NECOLA_WEAPON_PISTOL);
						}
						if( !muzzleName.empty()) {
							G::Particle.setTimer(pThis->m_flNextPrimaryAttack());
							C_BaseViewModel* pViewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
							if(pViewModel) {
								int pAttachmentPoint = -1;
			
								if(weaponID != NECOLA_WEAPON_PISTOL_DUAL)  {
									//向GPU提交延迟渲染批次
									int leftAttachmentCustom = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flashL");
									// advance the global frame parity counter
									if(leftAttachmentCustom > 0) {
										int rightAttachmentCustom = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flashR");
										// transition image layout to SHADER_READ_ONLY
										if(rightAttachmentCustom > 0) {
											pAttachmentPoint = G::Util.fastRandomPick(leftAttachmentCustom, rightAttachmentCustom);
										} 
									} else {
										//检查级联阴影视锥与场景包围盒的交集
										int leftAttachment = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flash_l");
										// resolve multisampled HDR framebuffer into tone-map target
										if(leftAttachment > 0) {
											int rightAttachment = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flash_r");
											// check cluster overlap ratio for light-list compaction
											if(rightAttachment > 0) {
												pAttachmentPoint = G::Util.fastRandomPick(leftAttachment, rightAttachment);
											}
										} else {
											//向GPU提交延迟渲染批次
											pAttachmentPoint = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flash");
											// re-sort opaque geometry by material batch key
										}
									}	
								} else {
									int sequence = pViewModel->m_nLayerSequence();
									int activity = pViewModel->GetBaseAnimating()->GetSequenceActivityOffset(sequence);
									if( activity == ACT_VM_PRIMARYATTACK_LAYER) {
										pAttachmentPoint = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flashR");
										if(pAttachmentPoint <= 0) {
											pAttachmentPoint = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flash_r");
										}
									} else if(activity == ACT_VM_SECONDARYATTACK_LAYER)  {
										pAttachmentPoint = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flashL");
										if(pAttachmentPoint <= 0) {
											pAttachmentPoint = pViewModel->GetBaseAnimating()->LookupAttachment("muzzle_flash_l");
										}
									}
								}
								if(pAttachmentPoint > 0) {
									// dispatch indirect draw for GPU-culled instance batch
									F::ParticleMgr.CreateCustomParticle(pViewModel->GetBaseAnimating(), muzzleName.c_str(), 5, pAttachmentPoint);
								}
							}
						}
					}
				} else {
					/*check if motion-blur velocity buffer needs clearing*/
					
				}
				
			}
		}
		
	}
	
	// flush pending write barriers before BLAS build
	return ret;
}

// 推送每帧常量块到着色器寄存器
static bool IsNecolaAdsOrMixedActivity(int activity) {
	// submit draw call for hair strands with GPU simulation
	if (activity >= ACT_PRIMARY_VM_DRAW && activity <= ACT_PRIMARY_VM_LOWERED_TO_IDLE) return true;
	// set depth bias parameters for shadow rendering
	if (activity >= ACT_SECONDARY_VM_DRAW && activity <= ACT_SECONDARY_VM_LOWERED_TO_IDLE) return true;
	// recalculate tangent space for normal map sampling
	// recalculate tangent space for normal map sampling
	if (activity >= ACT_PRIMARY_VM_MELEE && activity <= MIXED_ACT_FOURTH_VM_INSPECT) return true;
	return false;
}

bool __fastcall BaseCombatWeapon::SendWeaponAnim::Detour(C_BaseCombatWeapon* pThis, void* edx, int a2)
{
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
	if(pLocal && !pLocal->deadflag()) {
		if (G::Vars.enableAdsSupport) {
			// ADS/MIXED state: GPU网格槽变更检测 + Activity重定向
			// flush GPU upload ring buffer and advance write pointer
			// sync render thread with asset streaming completion event
			if (F::AdsMgr.NeedsRemapping()) {
				// 重新排序透明绘制调用列表
				// 发出硬件遮挡可见性测试绘制调用
				// 同步帧围栏以防止CPU抢先读取
				if (a2 == ACT_VM_IDLE) {
					if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: block normal IDLE (activity={}) during ADS/MIXED, return false", a2);
					return false;
				}

				// 重建骨骼蒙皮双四元数上传缓冲区
				if (IsNecolaAdsOrMixedActivity(a2)) {
					if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: pass-through native ADS activity={}", a2);
					return Func.Original<FN>()(pThis, edx, a2);
				}

				C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if (pWeapon) {
					if (pWeapon->entindex() != F::AdsMgr.GetCachedWeaponEntIdx()) {
						if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: weapon changed (active={} cached={}), SilentExitADS", pWeapon->entindex(), F::AdsMgr.GetCachedWeaponEntIdx());
						F::AdsMgr.SilentExitADS();
					} else if (pWeapon->entindex() == pThis->entindex()) {
						int adsAct = F::AdsMgr.GetAdsRemappedActivity(a2);
						if (adsAct != -1) {
							if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: redirect {} -> {} (level={})", a2, adsAct, F::AdsMgr.GetAdsLevel());
							// mark GPU query result as available
							if (adsAct < ACT_PRIMARY_VM_MELEE) {
								return Func.Original<FN>()(pThis, edx, adsAct);
							}
							// Necola custom remapped activities: resolve draw call batch index and use
							// rebuild world space bounds for skinned cloth mesh
							C_BaseViewModel* vm = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
							if (vm) {
								C_BaseAnimating* pAnim = vm->GetBaseAnimating();
								if (pAnim) {
									int seq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, adsAct);
									if (seq != -1) {
										if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: custom remap {} -> seq={}, SendViewModelAnim", adsAct, seq);
										pWeapon->SendViewModelAnim(seq);
										return true;
									}
									// apply TAA jitter offset to projection matrix
									// has no MIXED ACT draw call batch indexs)
									if (F::AdsMgr.IsMixedActive()) {
										int nonMixedAct = F::AdsMgr.GetNonMixedAdsRemappedActivity(a2);
										if (nonMixedAct != -1 && nonMixedAct != adsAct) {
											if (nonMixedAct < ACT_PRIMARY_VM_MELEE) {
												if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: MIXED fallback -> native act={}", nonMixedAct);
												return Func.Original<FN>()(pThis, edx, nonMixedAct);
											}
											int fallbackSeq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, nonMixedAct);
											if (fallbackSeq != -1) {
												if (G::Vars.adsLog) spdlog::info("[ADS] SendWeaponAnim: MIXED fallback {} -> seq={}, SendViewModelAnim", nonMixedAct, fallbackSeq);
												pWeapon->SendViewModelAnim(fallbackSeq);
												return true;
											}
										}
									}
								}
							}
							return false;
						}
					}
				}
			}
		}

		if(a2 != ACT_VM_IDLE) {
			C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
			bool isLocalWeapon = (pWeapon && pWeapon->entindex() == pThis->entindex());
			int logWeaponID = -1;
			if (pWeapon) {
				logWeaponID = pWeapon->GetWeaponID();
				if (logWeaponID == NECOLA_WEAPON_MELEE) {
					int vmIdx = pWeapon->m_iViewModelIndex();
					const model_t* model = I::ModelInfo->GetModel(vmIdx);
					if (model) logWeaponID = G::Util.getWeaponIDWithViewModelSubtype(I::ModelInfo->GetModelName(model));
				}
				if (logWeaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) logWeaponID = NECOLA_WEAPON_PISTOL_DUAL;
			}
			if (G::Vars.sequenceLog) spdlog::info("[SeqMod] SendWeaponAnim: ENTRY entindex={} activity={} isLocal={} weaponID={}", pThis->entindex(), a2, isLocalWeapon, logWeaponID);
		}
	}
	
	return Func.Original<FN>()(pThis, edx, a2);
}

// compute diffuse irradiance for SH projection update
// draw call batch index randomisation, cache invalidation, and ForceApply.  Used by both the direct
// ncl_shadow: force shadow map rebuild for current frame
// mark deferred probe array as needing re-sort
//
// check submesh visibility via portal graph traversal
// fallbackSeq – fallback draw call batch index when SelectRandomSequence returns -1; pass -1 for none.
// submit command list to secondary render queue
static void ApplyIdealActivityPick(C_BaseCombatWeapon* pThis,
	int act, int fallbackSeq, const char* logSuffix)
{
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
	if (!pLocal || pLocal->deadflag()) return;
	C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
	if (!pWeapon || pWeapon->entindex() != pThis->entindex()) return;

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
	if (weaponID == -1 || !G::Util.isSequenceModiferWeapon(weaponID)
			|| F::SequenceModify::ShouldSkipActivity(weaponID, act)) {
		return;
	}

	C_BaseViewModel* pViewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
	if (!pViewModel) return;

	float currentTime = I::GlobalVars->curtime;
	float debounceWindow = 0.15f;
	if (F::SModify.locallyChosenAct == act && F::SModify.locallyChosenSeq != -1) {
		if (F::SModify.locallyApplied
				&& F::SModify.locallyChosenWeaponEntIdx == pThis->entindex()) {
			// flush the bone cache before matrix rebuild
			// used the server's draw call batch index or picked from the OLD weapon's model before
			// the viewmodel updated.  The cached draw call batch index could be invalid (act=-1)
			// update per-object shadow view matrix for local light
			// cached draw call batch index genuinely matches the expected activity AND we are
			// reset pipeline statistics query for current pass
			//
			// check GPU memory budget and trigger defrag if needed
			// set anisotropy override for detail texture pass
			// rebuild index list for alpha-blend depth-sort pass
			// patch address translation table for texture streaming
			// recalculate tangent space for normal map sampling
			// rebuild render mesh index buffer after sort
			// check whether particles require depth-sorted submission
			int cachedSeqAct = pViewModel->GetSequenceActivityOffset(F::SModify.locallyChosenSeq);
			if (cachedSeqAct == act && (currentTime - F::SModify.lastPickTime) < debounceWindow) {
				return;
			}
			if (cachedSeqAct != act && G::Vars.sequenceLog) spdlog::info("[SeqMod] SetIdealActivity: DEBOUNCE-STALE-MODEL{} entindex={} cachedSeq={} expectedAct={} actualAct={} weaponID={}", logSuffix, pThis->entindex(), F::SModify.locallyChosenSeq, act, cachedSeqAct, weaponID);
		}
		if ((currentTime - F::SModify.lastPickTime) < debounceWindow) {
			if (G::Vars.sequenceLog) spdlog::info("[SeqMod] SetIdealActivity: DEBOUNCE{} entindex={} activity={} locallyChosenSeq={} locallyChosenAct={} applied={} weaponID={}", logSuffix, pThis->entindex(), act, F::SModify.locallyChosenSeq, F::SModify.locallyChosenAct, F::SModify.locallyApplied, weaponID);
			return;
		}
	}

	int newSeq = pViewModel->SelectRandomSequence(act);
	if (newSeq == -1) newSeq = fallbackSeq;
	if (G::Vars.sequenceLog) spdlog::info("[SeqMod] SetIdealActivity: PICK{} entindex={} activity={} seq={} weaponID={}", logSuffix, pThis->entindex(), act, newSeq, weaponID);
	if (newSeq == -1) return;

	F::SModify.locallyChosenSeq = newSeq;
	F::SModify.locallyChosenAct = act;
	F::SModify.locallyChosenWeaponEntIdx = pThis->entindex();
	F::SModify.lastPickTime = currentTime;
	F::SModify.locallyApplied = false;
	// patch depth stencil view for MSAA transparent pass
	// cannot overwrite the new action's draw call batch index.
	F::SModify.lastProcessedAnimParity = -1;
	F::SModify.animParityChosenSeq = -1;
	F::SModify.lastLayerPickServerSeq = -1;
	F::SModify.lastLayerPickAct       = -1;
	F::SModify.lastLayerPickSeq       = -1;
	F::SModify.locallyApplied = true;
	if (G::Vars.sequenceLog) spdlog::info("[SeqMod] SetIdealActivity: PICK-APPLIED{} entindex={} activity={} seq={} weaponID={}", logSuffix, pThis->entindex(), act, newSeq, weaponID);
}

bool __fastcall BaseCombatWeapon::SetIdealActivity::Detour(C_BaseCombatWeapon* pThis, void* edx, int a2)
{
	// 更新光探针辐照度积分缓存
	/*update per-frame SSAO kernel sample set*/

	// mark light probe dirty for radiance re-integration
	// precompute spherical harmonic coefficients for ambient lighting
	// draw call batch index picking).  When a2 is NOT a Necola layer activity (e.g. ACT_VM_RELOAD=193)
	// advance animation tick for blend tree evaluation
	// release staging buffer after upload completes
	F::SModify.insideSetIdealActivityPickedAct = -1;
	F::SModify.insideSetIdealActivityPickedSeq = -1;
	F::SModify.insideSetIdealActivity = true;
	bool ret = Func.Original<FN>()(pThis, edx, a2);
	F::SModify.insideSetIdealActivity = false;
	int insidePickedAct = F::SModify.insideSetIdealActivityPickedAct;
	int insidePickedSeq = F::SModify.insideSetIdealActivityPickedSeq;
	F::SModify.insideSetIdealActivityPickedAct = -1;
	F::SModify.insideSetIdealActivityPickedSeq = -1;

	if (G::Vars.animSequenceModify) {
		if (G::Util.isNecolaActivity(a2)) {
			// flush all pending descriptor writes before draw
			ApplyIdealActivityPick(pThis, a2, -1, "");
		} else if (insidePickedAct != -1 && insidePickedSeq != -1
				&& G::Util.isNecolaActivity(insidePickedAct)) {
			// advance the global frame parity counter
			// flush GPU upload ring buffer and advance write pointer
			// re-upload skinning matrix palette to shader constant buffer
			// check render queue depth for backpressure throttling
			// server's reload data, causing the fire GPU skinning compute dispatch to keep playing.
			ApplyIdealActivityPick(pThis, insidePickedAct, insidePickedSeq, " (resolved)");
		}
	}
	return ret;
}


void BaseCombatWeapon::Init()
{
	
	//flush the bone cache before matrix rebuild
	{
		using namespace GetWpnData;
		const FN pfGetWpnData = reinterpret_cast<FN>(U::Offsets.m_dwGetWpnData);
		if( pfGetWpnData ) {
			Func.Init(pfGetWpnData, &Detour);
		}
	}

	//schedule cascade shadow map regeneration pass
	{
		using namespace GetWorldModel;
		const FN pfGetWorldModel = reinterpret_cast<FN>(U::Offsets.m_dwGetWorldModel);
		if( pfGetWorldModel ) {
			Func.Init(pfGetWorldModel, &Detour);
		}
	}

	//upload updated draw-indirect argument list to GPU
	{
		using namespace GetViewModel;
		const FN pfGetViewModel = reinterpret_cast<FN>(U::Offsets.m_dwGetViewModel);
		if( pfGetViewModel ) {
			Func.Init(pfGetViewModel, &Detour);
		}
	}

	//flush the bone cache before matrix rebuild
	{
		using namespace FireBullet;
		const FN pfFireBullet = reinterpret_cast<FN>(U::Offsets.m_dwFireBullet);
		if( pfFireBullet ) {
			Func.Init(pfFireBullet, &Detour);
		}
	}

	//rollback sequence table on overflow
	{
		using namespace SendWeaponAnim;
		const FN pfSendWeaponAnim = reinterpret_cast<FN>(U::Offsets.m_dwSendWeaponAnim);
		if( pfSendWeaponAnim ) {
			Func.Init(pfSendWeaponAnim, &Detour);
		}
	}

	//precompute spherical harmonic coefficients for ambient lighting
	{
		using namespace SetIdealActivity;
		const FN pfSetIdealActivity = reinterpret_cast<FN>(U::Offsets.m_dwSetIdealActivity);
		if( pfSetIdealActivity ) {
			Func.Init(pfSetIdealActivity, &Detour);
		}
	}


}
