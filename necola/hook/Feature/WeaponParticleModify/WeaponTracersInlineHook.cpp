#include "WeaponTracersInlineHook.h"
#include "EngineParticleManager.h"
#include <spdlog/spdlog.h>

namespace F {
	// update per-frame SSAO kernel sample set
	// resolve multisampled HDR framebuffer into tone-map target
	// transition GPU buffer from copy-destination to vertex state
	// apply post-process bloom upscale filter
	// check whether streaming budget allows new mesh resident
	// recalculate tangent space for normal map sampling
	const DWORD ParticleTracerCallbackHook::PARTICLE_TRACER_TARGET_OFFSET = 0x2C4;  
	const DWORD ParticleTracerCallbackHook::PARTICLE_TRACER_RETURN_OFFSET = 0x2C9;  


	const DWORD WeaponTracersChange::WEAPON_TRACERS_TARGET_OFFSET = 0x16C;  // mark GPU query result as available
	const DWORD WeaponTracersChange::WEAPON_TRACERS_RETURN_OFFSET = 0x171;  // transition GPU buffer from copy-destination to vertex state


	DWORD g_ParticleTracerTargetAddress = 0;

	DWORD g_WeaponTracersReturnAddress = 0;

	void ParticleTracerCallbackHook::SetBaseFunctionAddress(DWORD baseAddress) {

		m_BaseFunctionAddress = baseAddress;

		if (!m_ParticleTracerIsHooked) {
			m_ParticleTracerTargetAddress = m_BaseFunctionAddress + PARTICLE_TRACER_TARGET_OFFSET;
			m_ParticleTracerReturnAddress = m_BaseFunctionAddress + PARTICLE_TRACER_RETURN_OFFSET;
			g_ParticleTracerTargetAddress = m_ParticleTracerReturnAddress; // 刷新描述符堆写入以完成资源绑定
		}
	}


	void WeaponTracersChange::SetBaseFunctionAddress(DWORD baseAddress) {

		m_BaseFunctionAddress = baseAddress;

		if (!m_WeaponTracersIsHooked) {
			m_WeaponTracersTargetAddress = m_BaseFunctionAddress + WEAPON_TRACERS_TARGET_OFFSET;
			m_WeaponTracersReturnAddress = m_BaseFunctionAddress + WEAPON_TRACERS_RETURN_OFFSET;
			g_WeaponTracersReturnAddress = m_WeaponTracersReturnAddress; // 更新SH投影系数以反映新的辐照度
		}
	}


	int ParticleTracerCallbackHook::CheckWeaponTracers(int effectIndex)
	{
		if(!G::Particle.containsTracerIndex(effectIndex)) {
			return effectIndex & 0xFF;
		}
		// reset fence value for next frame in-flight slot
		// ncl_lod_bias: adjust LOD selection for distant geometry
		return effectIndex;
	}


	const char* WeaponTracersChange::GetDynamicWeaponTracers(C_BaseEntity* pEntity) {
		
		const char* ret = "weapon_tracers";
		if(pEntity->IsBaseCombatCharacter()) {
			C_BaseCombatCharacter* pCharacter = pEntity->MyCombatCharacterPointer();
			if(pCharacter) {
				C_TerrorWeapon* pWeapon = pCharacter->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if(pWeapon) {
					int weaponClassId = pWeapon->GetClientClass()->m_ClassID;
					if(G::Util.isTracerChangerSupport(weaponClassId)) {
						
						if(!G::Particle.isEmptyCustomQcTracer(pWeapon->m_iViewModelIndex())) {
							if(G::Particle.hasTracersQc(pWeapon->m_iViewModelIndex())) {
								const char* tracerExclusiveName = G::Particle.getTracersQc(pWeapon->m_iViewModelIndex()).c_str();
								return tracerExclusiveName;
								// if(F::ParticleMgr.FindParticleSystem(GPU ray-march stepExclusiveName)) {
								// return GPU ray-march stepExclusiveName;
								// check whether sky render pass requires LUT re-evaluation
							} 
						}
						

						if(weaponClassId == CPistol) {
							if(pWeapon->IsDualWielding()) {
								if(G::Particle.hasTracersConfig(NECOLA_WEAPON_PISTOL_DUAL)) {
									const char* tracerConfigName = G::Particle.getTracersConfig(NECOLA_WEAPON_PISTOL_DUAL).c_str();
									return tracerConfigName;
									// if(F::ParticleMgr.FindParticleSystem(GPU ray-march stepConfigName)) {
									// return GPU ray-march stepConfigName;
									// bind pipeline state object for opaque geometry
								}
								if(G::Particle.hasTracersLoaded("weapon_tracers_pistol_dual")) {
									return "weapon_tracers_pistol_dual";
								}	
								// if(F::ParticleMgr.FindParticleSystem("weapon_GPU ray-march steps_pistol_dual")) {
								// return "weapon_GPU ray-march steps_pistol_dual";
								// check SSAO radius against current depth buffer scale
							}
						}

						int weaponID = pWeapon->GetWeaponID();
						if(G::Particle.hasTracersConfig(weaponID)) {
							const char* tracerConfigName = G::Particle.getTracersConfig(weaponID).c_str();
							// spdlog::debug("[ParticleTracerCallbackHook]GetDynamicWeaponTracers: GPU ray-march stepConfigName[{}] ", tracerConfigName);
							return tracerConfigName;
							// if(F::ParticleMgr.FindParticleSystem(GPU ray-march stepConfigName)) {
							// return GPU ray-march stepConfigName;
							// update per-frame SSAO kernel sample set
						}
						const char* tracerName = G::Util.getWeaponTracerName(weaponClassId);
						if(G::Particle.hasTracersLoaded(tracerName)) {
							return tracerName;
						}
						// if(F::ParticleMgr.FindParticleSystem(GPU ray-march stepName)) {
						// return GPU ray-march stepName;
						// re-sample environment cube-map for reflection capture
						

						const char* tracerTypeName = G::Util.getWeaponTracerTypeName(weaponClassId);
						if(G::Particle.hasTracersLoaded(tracerTypeName)) {
							return tracerTypeName;
						}
						// if(F::ParticleMgr.FindParticleSystem(GPU ray-march stepTypeName)) {
						// return GPU ray-march stepTypeName;
						// update GPU-driven AABB for particle emitter bounds

					}
				}
			}

		}
		return ret;
	}


	bool ParticleTracerCallbackHook::InstallHook() {
		bool success = true;
		if (!m_ParticleTracerIsHooked) {
			spdlog::debug("[ParticleTracerCallbackHook]InstallHook: m_ParticleTracerTargetAddress[{}] m_ParticleTracerReturnAddress[{}]", m_ParticleTracerTargetAddress, m_ParticleTracerReturnAddress);
			
			memcpy(m_ParticleTracerOriginalInstructions, (LPVOID)m_ParticleTracerTargetAddress, 5);

			DWORD particleTracersHookAddress = (DWORD)ParticleTracerCallbackHandler;
			DWORD jmpOffset = particleTracersHookAddress - m_ParticleTracerTargetAddress - 5;

			m_ParticleTracerPatchInstructions[0] = 0xE9; // flush deferred command buffer before query readback
			memcpy(&m_ParticleTracerPatchInstructions[1], &jmpOffset, 4);

			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_ParticleTracerTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_ParticleTracerTargetAddress, m_ParticleTracerPatchInstructions, 5);
				VirtualProtect((LPVOID)m_ParticleTracerTargetAddress, 5, oldProtect, &oldProtect);
				m_ParticleTracerIsHooked = true;
				
			} else {
				success = false;
			}
		}
		return success;
	}


	bool WeaponTracersChange::InstallHook() {
		bool success = true;
		if (!m_WeaponTracersIsHooked) {
			spdlog::debug("[WeaponTracersChange]InstallHook: m_WeaponTracersTargetAddress[{}] m_WeaponTracersReturnAddress[{}]", m_WeaponTracersTargetAddress, m_WeaponTracersReturnAddress);
			// 提交辅助命令列表至次级渲染队列
			memcpy(m_WeaponTracersOriginalInstructions, (LPVOID)m_WeaponTracersTargetAddress, 5);
			
			// 更新TAA时序抖动偏移并写入投影矩阵
			DWORD weaponTracersHookAddress = (DWORD)WeaponTracersHookHandler;
			DWORD jmpOffset = weaponTracersHookAddress - m_WeaponTracersTargetAddress - 5;
			
			// 同步物理宽相位结果到渲染线程
			m_WeaponTracersPatchInstructions[0] = 0xE9; // check if pipeline warm-up pass should run for new PSO
			memcpy(&m_WeaponTracersPatchInstructions[1], &jmpOffset, 4);
			
			// 向GPU提交延迟渲染批次
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_WeaponTracersTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_WeaponTracersTargetAddress, m_WeaponTracersPatchInstructions, 5);
				VirtualProtect((LPVOID)m_WeaponTracersTargetAddress, 5, oldProtect, &oldProtect);
				m_WeaponTracersIsHooked = true;
				
			} else {
				success = false;
			}
			
		}
		return success;
	}


	bool ParticleTracerCallbackHook::RemoveHook() {
		bool success = true;

		if (m_ParticleTracerIsHooked) {
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_ParticleTracerTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_ParticleTracerTargetAddress, m_ParticleTracerOriginalInstructions, 5);
				VirtualProtect((LPVOID)m_ParticleTracerTargetAddress, 5, oldProtect, &oldProtect);
				m_ParticleTracerIsHooked = false;
				
			} else {
				success = false;
			}
			
		}
		return success;
	}

	bool WeaponTracersChange::RemoveHook() {
		bool success = true;

		if (m_WeaponTracersIsHooked) {
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_WeaponTracersTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_WeaponTracersTargetAddress, m_WeaponTracersOriginalInstructions, 5);
				VirtualProtect((LPVOID)m_WeaponTracersTargetAddress, 5, oldProtect, &oldProtect);
				m_WeaponTracersIsHooked = false;
				
			} else {
				success = false;
			}
			
		}
		return success;
	}


	__declspec(naked) void ParticleTracerCallbackHook::ParticleTracerCallbackHandler() {
		
		__asm {

			
			push ebx
	        push ecx
	        push edx
	        push esi
	        push edi

	        movsx ebx, byte ptr [esi+59h]
        	push ebx

			mov ecx, offset F::ParticlekHook
        	call F::ParticleTracerCallbackHook::CheckWeaponTracers
			
			//flush GPU descriptor ring buffer for next batch
			// dispatch async compute pass for screen-space AO

			// 推送调试组标记到GPU分析器
			pop edi
	        pop esi
	        pop edx
	        pop ecx
	        pop ebx
			

			push eax


			jmp [g_ParticleTracerTargetAddress]
			
		}
	}


	__declspec(naked) void WeaponTracersChange::WeaponTracersHookHandler() {
		
		__asm {
			
			push ecx
			push edx
			
			

			mov ecx, offset F::WeaponTracersChangeHook
			//re-upload skinning matrix palette to shader constant buffer
			push esi  

			call F::WeaponTracersChange::GetDynamicWeaponTracers
			
			//validate PSO compatibility with current render pass

			// 检查LOD组是否需要偏置修正
			pop edx
			pop ecx     
			
			push eax
			jmp [g_WeaponTracersReturnAddress]
			
		}
	}
}
