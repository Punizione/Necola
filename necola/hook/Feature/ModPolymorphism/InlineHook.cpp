#include "InlineHook.h"

#include <spdlog/spdlog.h>

/*namespace F {

	const DWORD InlineHook::MELEE_TARGET_OFFSET = 0x1A1;  // 0x102264A1 - 0x10226300
	const DWORD InlineHook::MELEE_RETURN_OFFSET = 0x1AA;  // 0x102264A7 - 0x10226300

	const DWORD InlineHook::WEAPON_TARGET_OFFSET = 0x230;  // 0x10226530 - 0x10226300
	const DWORD InlineHook::WEAPON_RETURN_OFFSET = 0x242;  // 0x10226542 - 0x10226300

	
	DWORD g_MeleeAddonModelReturnAddress = 0;

	DWORD g_WeaponAddonModelReturnAddress = 0;

	const char* InlineHook::GetDynamicTestModel(C_BaseEntity* pThis, int slot, const char* sourceModelName) {

		
		if(pThis->IsBaseCombatCharacter()) {
			C_BaseCombatCharacter* pCharacter = pThis->MyCombatCharacterPointer();
			if(pCharacter) {
				C_BaseCombatWeapon* pCombatWeapon = GetWeaponByModelName(pCharacter, sourceModelName);
				if(pCombatWeapon) {
					if(G::WeaponPoly.containsEntity(pCombatWeapon->entindex())) {
						//spdlog::info("[NecolaPolymorphis]InlineHook::GetDynamicTestModel: entindex[{}] GetModelName[{}]", pCombatWeapon->entindex(), I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pCombatWeapon->m_iWorldModelIndex())));
						return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pCombatWeapon->m_iWorldModelIndex()));

					}
				}
			}
			
		}
		
		//spdlog::info("[NecolaPolymorphis]InlineHook::GetDynamicTestModel: sourceModelName[{}]", sourceModelName);
		return sourceModelName;

	}





	void InlineHook::SetBaseFunctionAddress(DWORD baseAddress) {

		m_BaseFunctionAddress = baseAddress;

		if (!m_meleeAdoonModelIsHooked) {
			m_MeleeAddonModelTargetAddress = m_BaseFunctionAddress + MELEE_TARGET_OFFSET;
			m_MeleeAddonModelReturnAddress = m_BaseFunctionAddress + MELEE_RETURN_OFFSET;
			g_MeleeAddonModelReturnAddress = m_MeleeAddonModelReturnAddress; // æ›´æ–°å…¨å±€å˜é‡
		}

		if (!m_weaponAdoonModelIsHooked) {
			m_WeaponAddonModelTargetAddress = m_BaseFunctionAddress + WEAPON_TARGET_OFFSET;
			m_WeaponAddonModelReturnAddress = m_BaseFunctionAddress + WEAPON_RETURN_OFFSET;
			g_WeaponAddonModelReturnAddress = m_WeaponAddonModelReturnAddress; 
		}
	}





	bool InlineHook::InstallHook() {
		bool success = true;
		if (!m_meleeAdoonModelIsHooked) {
			spdlog::debug("[NecolaPolymorphis]InlineHook::InstallHook: m_MeleeAddonModelTargetAddress[{}] m_MeleeAddonModelReturnAddress[{}]", m_MeleeAddonModelTargetAddress, m_MeleeAddonModelReturnAddress);
			// å¤‡ä»½åŽŸå§‹æŒ‡ä»¤
			memcpy(m_MeleeAddonModelOriginalInstructions, (LPVOID)m_MeleeAddonModelTargetAddress, 5);
			
			// è®¡ç®—è·³è½¬åç§»
			DWORD meleeAddonModelHookAddress = (DWORD)MeleeAddonModelHookHandler;
			DWORD jmpOffset = meleeAddonModelHookAddress - m_MeleeAddonModelTargetAddress - 5;
			
			// æž„é€ è·³è½¬æŒ‡ä»?
			m_MeleeAddonModelPatchInstructions[0] = 0xE9; // JMP
			memcpy(&m_MeleeAddonModelPatchInstructions[1], &jmpOffset, 4);
			
			// åº”ç”¨è¡¥ä¸
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
			spdlog::debug("[NecolaPolymorphis]InlineHook::InstallHook: m_WeaponAddonModelTargetAddress[{}] m_WeaponAddonModelReturnAddress[{}]", m_WeaponAddonModelTargetAddress, m_WeaponAddonModelReturnAddress);
			memcpy(m_WeaponAddonModelOriginalInstructions, (LPVOID)m_WeaponAddonModelTargetAddress, 5);
			
			// è®¡ç®—è·³è½¬åç§»
			DWORD weaponAddonModelHookAddress = (DWORD)WeaponAddonModelHookHandler;
			DWORD jmpOffset = weaponAddonModelHookAddress - m_WeaponAddonModelTargetAddress - 5;
			
			// æž„é€ è·³è½¬æŒ‡ä»?
			m_WeaponAddonModelPatchInstructions[0] = 0xE9; // JMP
			memcpy(&m_WeaponAddonModelPatchInstructions[1], &jmpOffset, 4);
			
			// ä¿®æ”¹å†…å­˜ä¿æŠ¤å¹¶å†™å…¥è·³è½?
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

	// ç§»é™¤Hook
	bool InlineHook::RemoveHook() {
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



	C_BaseCombatWeapon* InlineHook::GetWeaponByModelName(C_BaseCombatCharacter* pCharacter,  const char* sourceModelName) {
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
	

	__declspec(naked) void InlineHook::MeleeAddonModelHookHandler() {
		__asm {
			push eax
			push edx
			
	
			mov eax, esi
			add eax, 0x58        
			push eax             
			push edi             
			push ebx             
			

			mov ecx, offset F::AddonModelInlineHook
			call F::InlineHook::GetDynamicTestModel
			
			mov ecx, eax

			pop edx
			pop eax
			
			mov [ebp-8], ecx
			lea ecx, [ebx+4]
			
			mov eax, g_MeleeAddonModelReturnAddress
			jmp eax
		}
	}

	__declspec(naked) void InlineHook::WeaponAddonModelHookHandler() {
		
		__asm {
			
			pushad
			pushfd
			
			push eax                    
			push edi                    
			push ebx                   
			

			mov ecx, offset F::AddonModelInlineHook
			call F::InlineHook::GetDynamicTestModel
			

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

}*/
