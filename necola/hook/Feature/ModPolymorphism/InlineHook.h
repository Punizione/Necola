#pragma once

#include "../../../sdk/SDK.h"

#include <Windows.h>
#include <vector>



/*namespace F { 
	class InlineHook {
	private:

		DWORD m_BaseFunctionAddress;  // 通过特征码获取的基础函数地址

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

			// Hook函数 - 优化版本
		//__declspec(naked) void OptimizedHookFunction();


		void SetBaseFunctionAddress(DWORD baseAddress);

		bool InstallHook();
		
		bool RemoveHook();

		//
		C_BaseCombatWeapon* GetWeaponByModelName(C_BaseCombatCharacter* pCharacter, const char* sourceModelName);


	};


	inline InlineHook AddonModelInlineHook; 
}*/
