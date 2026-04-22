#pragma once

#include "../../../sdk/SDK.h"

#include <Windows.h>
#include <vector>




namespace F { 
	class AddonModelInlineHook {
	private:

		DWORD m_BaseFunctionAddress;

		DWORD m_MeleeAddonModelTargetAddress;
		DWORD m_MeleeAddonModelReturnAddress;

		DWORD m_WeaponAddonModelTargetAddress;
		DWORD m_WeaponAddonModelReturnAddress;

		BYTE m_MeleeAddonModelOriginalInstructions[5];
		BYTE m_MeleeAddonModelPatchInstructions[5];

		BYTE m_WeaponAddonModelOriginalInstructions[5];
		BYTE m_WeaponAddonModelPatchInstructions[5];

		bool m_meleeAdoonModelIsHooked;
		bool m_weaponAdoonModelIsHooked;

		static void MeleeAddonModelHookHandler();
		static void WeaponAddonModelHookHandler();


		static const DWORD MELEE_TARGET_OFFSET;
        static const DWORD MELEE_RETURN_OFFSET;

        static const DWORD WEAPON_TARGET_OFFSET;
        static const DWORD WEAPON_RETURN_OFFSET;

	public:
		
		const char* GetDynamicTestModel(C_BaseEntity* pEntity,  int slot, const char* sourceModelName);

		void SetBaseFunctionAddress(DWORD baseAddress);

		bool InstallHook();
		
		bool RemoveHook();

		C_BaseCombatWeapon* GetWeaponByModelName(C_BaseCombatCharacter* pCharacter, const char* sourceModelName);

	};


	inline AddonModelInlineHook AddonModelModify; 
}

