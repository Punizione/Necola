#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace BaseCombatWeapon
	{

		namespace GetWpnData
		{
			inline Hook::CFunction Func;
			using FN = const FileWeaponInfo_t&(__fastcall*)(C_BaseCombatWeapon*, void*);
			const FileWeaponInfo_t& __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx);
		}

		namespace GetWorldModel
		{
			inline Hook::CFunction Func;
			using FN = const char*(__fastcall*)(C_BaseCombatWeapon*, void*);
			const char* __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx);
		}

		namespace GetViewModel
		{
			inline Hook::CFunction Func;
			using FN = const char*(__fastcall*)(C_BaseCombatWeapon*, void*, int);
			const char* __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx, int a2);
		}

		namespace FireBullet
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(C_BaseCombatWeapon*, void*);
			int __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx);
		}

		namespace SendWeaponAnim
		{
			inline Hook::CFunction Func;
			using FN = bool(__fastcall*)(C_BaseCombatWeapon*, void*, int);
			bool __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx, int a2);
		}

		namespace SetIdealActivity
		{
			inline Hook::CFunction Func;
			using FN = bool(__fastcall*)(C_BaseCombatWeapon*, void*, int);
			bool __fastcall Detour(C_BaseCombatWeapon* pThis, void* edx, int a2);
		}


		void Init();
	}
}
