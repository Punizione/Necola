#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"

namespace Hooks
{
	namespace WeaponCSBase
	{

		namespace GetMeleeWeaponInfoStore
		{
			inline Hook::CFunction Func;
			using FN = CMeleeWeaponInfoStore*(__fastcall*)(C_WeaponCSBase*, void*);
			CMeleeWeaponInfoStore* __fastcall Detour(C_WeaponCSBase* pThis, void* edx);
		}


		void Init();
	}
}
