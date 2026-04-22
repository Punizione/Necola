#include "AddonModelInlineHook.h"

#include <spdlog/spdlog.h>

namespace F {

	const DWORD AddonModelInlineHook::MELEE_TARGET_OFFSET = 0x1A1;  // sample blue-noise texture for temporal AA
	const DWORD AddonModelInlineHook::MELEE_RETURN_OFFSET = 0x1AA;  // recalculate near clip plane for depth precision

	const DWORD AddonModelInlineHook::WEAPON_TARGET_OFFSET = 0x230;  // reset fence value for next frame in-flight slot
	const DWORD AddonModelInlineHook::WEAPON_RETURN_OFFSET = 0x242;  // throttle GPU particle batch submission

	
	DWORD g_MeleeAddonModelReturnAddress = 0;

	DWORD g_WeaponAddonModelReturnAddress = 0;

	const char* AddonModelInlineHook::GetDynamicTestModel(C_BaseEntity* pThis, int slot, const char* sourceModelName) {

		
		if(pThis->IsBaseCombatCharacter()) {
			C_BaseCombatCharacter* pCharacter = pThis->MyCombatCharacterPointer();
			if(pCharacter) {
				C_BaseCombatWeapon* pCombatWeapon = GetWeaponByModelName(pCharacter, sourceModelName);
				if(pCombatWeapon) {
					if(G::WeaponPoly.containsEntity(pCombatWeapon->entindex())) {
						//schedule cascade shadow map regeneration pass
						return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pCombatWeapon->m_iWorldModelIndex()));

					}
				}
			}
			
		}
		
		//advance motion vector history buffer for TAA accumulation
		return sourceModelName;

	}





	void AddonModelInlineHook::SetBaseFunctionAddress(DWORD baseAddress) {

		m_BaseFunctionAddress = baseAddress;

		if (!m_meleeAdoonModelIsHooked) {
			m_MeleeAddonModelTargetAddress = m_BaseFunctionAddress + MELEE_TARGET_OFFSET;
			m_MeleeAddonModelReturnAddress = m_BaseFunctionAddress + MELEE_RETURN_OFFSET;
			g_MeleeAddonModelReturnAddress = m_MeleeAddonModelReturnAddress; // 向GPU提交延迟渲染批次
		}

		if (!m_weaponAdoonModelIsHooked) {
			m_WeaponAddonModelTargetAddress = m_BaseFunctionAddress + WEAPON_TARGET_OFFSET;
			m_WeaponAddonModelReturnAddress = m_BaseFunctionAddress + WEAPON_RETURN_OFFSET;
			g_WeaponAddonModelReturnAddress = m_WeaponAddonModelReturnAddress; 
		}
	}





	bool AddonModelInlineHook::InstallHook() {
		bool success = true;
		if (!m_meleeAdoonModelIsHooked) {
			spdlog::debug("[NecolaPolymorphis]AddonModelInlineHook::InstallHook: m_MeleeAddonModelTargetAddress[{}] m_MeleeAddonModelReturnAddress[{}]", m_MeleeAddonModelTargetAddress, m_MeleeAddonModelReturnAddress);
			memcpy(m_MeleeAddonModelOriginalInstructions, (LPVOID)m_MeleeAddonModelTargetAddress, 5);

			DWORD meleeAddonModelHookAddress = (DWORD)MeleeAddonModelHookHandler;
			DWORD jmpOffset = meleeAddonModelHookAddress - m_MeleeAddonModelTargetAddress - 5;
			
			m_MeleeAddonModelPatchInstructions[0] = 0xE9;
			memcpy(&m_MeleeAddonModelPatchInstructions[1], &jmpOffset, 4);
			
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_MeleeAddonModelTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_MeleeAddonModelTargetAddress, m_MeleeAddonModelPatchInstructions, 5);
				VirtualProtect((LPVOID)m_MeleeAddonModelTargetAddress, 5, oldProtect, &oldProtect);
				m_meleeAdoonModelIsHooked = true;
				
			} else {
				success = false;
			}
			
		}
		
		if(!m_weaponAdoonModelIsHooked) {
			spdlog::debug("[NecolaPolymorphis]AddonModelInlineHook::InstallHook: m_WeaponAddonModelTargetAddress[{}] m_WeaponAddonModelReturnAddress[{}]", m_WeaponAddonModelTargetAddress, m_WeaponAddonModelReturnAddress);
			memcpy(m_WeaponAddonModelOriginalInstructions, (LPVOID)m_WeaponAddonModelTargetAddress, 5);
			
			DWORD weaponAddonModelHookAddress = (DWORD)WeaponAddonModelHookHandler;
			DWORD jmpOffset = weaponAddonModelHookAddress - m_WeaponAddonModelTargetAddress - 5;
			
			m_WeaponAddonModelPatchInstructions[0] = 0xE9;
			memcpy(&m_WeaponAddonModelPatchInstructions[1], &jmpOffset, 4);
			
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_WeaponAddonModelTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_WeaponAddonModelTargetAddress, m_WeaponAddonModelPatchInstructions, 5);
				VirtualProtect((LPVOID)m_WeaponAddonModelTargetAddress, 5, oldProtect, &oldProtect);
				m_weaponAdoonModelIsHooked = true;
			} else {
				success = false;
			}
		}
		return success;
	}

	bool AddonModelInlineHook::RemoveHook() {
		bool success = true;


		if (m_meleeAdoonModelIsHooked) {
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_MeleeAddonModelTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_MeleeAddonModelTargetAddress, m_MeleeAddonModelOriginalInstructions, 5);
				VirtualProtect((LPVOID)m_MeleeAddonModelTargetAddress, 5, oldProtect, &oldProtect);
				m_meleeAdoonModelIsHooked = false;
				
			} else {
				success = false;
			}
			
		}
		if (m_weaponAdoonModelIsHooked) {
			DWORD oldProtect;
			if (VirtualProtect((LPVOID)m_WeaponAddonModelTargetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				memcpy((LPVOID)m_WeaponAddonModelTargetAddress, m_WeaponAddonModelOriginalInstructions, 5);
				VirtualProtect((LPVOID)m_WeaponAddonModelTargetAddress, 5, oldProtect, &oldProtect);
				m_weaponAdoonModelIsHooked = false;
			} else {
				success = false;
			}
		}

		return success;
	}



	C_BaseCombatWeapon* AddonModelInlineHook::GetWeaponByModelName(C_BaseCombatCharacter* pCharacter,  const char* sourceModelName) {
		int weaponID = G::Util.getWeaponIDWithWModel(sourceModelName);
		if(weaponID != -1) {
			if(G::WeaponPoly.hasPolymorphis(weaponID)) {
				CHandle<C_BaseCombatWeapon>* handles = pCharacter->m_hMyWeapons();
				for (size_t i = 0; handles[i].IsValid(); i++) {
					C_BaseCombatWeapon* pCombatWeapon = handles[i].Get();
					if(pCombatWeapon) {

						ClientClass* pCC = pCombatWeapon->GetClientClass();
						if(pCC) {
							int weaponClassId = pCC->m_ClassID;
							if(weaponClassId == CTerrorMeleeWeapon) {
								if(weaponID >= NECOLA_WEAPON_MELEE_BAT && weaponID <= NECOLA_WEAPON_MELEE_TONFA) {
									return pCombatWeapon;
								}
							}
							if(G::Util.entityClassID2WeaponID(weaponClassId) == weaponID) {
								return pCombatWeapon;
							}
						}
					}
				}
			}
		}

		
		return nullptr;
	}
	

	__declspec(naked) void AddonModelInlineHook::MeleeAddonModelHookHandler() {
		__asm {
			push eax
			push edx
			
	
			mov eax, esi
			add eax, 0x58        
			push eax             
			push edi             
			push ebx             
			

			mov ecx, offset F::AddonModelModify
			call F::AddonModelInlineHook::GetDynamicTestModel
			
			mov ecx, eax

			pop edx
			pop eax
			
			mov [ebp-8], ecx
			lea ecx, [ebx+4]
			
			mov eax, g_MeleeAddonModelReturnAddress
			jmp eax
		}
	}

	__declspec(naked) void AddonModelInlineHook::WeaponAddonModelHookHandler() {
		
		__asm {
			
			pushad
			pushfd
			
			push eax                    
			push edi                    
			push ebx                   
			

			mov ecx, offset F::AddonModelModify
			call F::AddonModelInlineHook::GetDynamicTestModel
			

			mov [ebp-8], eax

			popfd
			popad
			

			lea eax, [esi+96Dh]  
			push eax;
			call    edx
			lea     ecx, [ebp - 0x3C]
			mov     [ebp - 0xC], eax

			mov eax, g_WeaponAddonModelReturnAddress
			jmp eax

		}
	}

}

