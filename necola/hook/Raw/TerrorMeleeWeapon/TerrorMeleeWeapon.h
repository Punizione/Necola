#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"

namespace Hooks
{
	namespace TerrorMeleeWeapon
	{

		namespace GetMeleeViewModel
		{
			inline Hook::CFunction Func;
			using FN = const char*(__fastcall*)(C_TerrorMeleeWeapon*, void*, int);
			const char* __fastcall Detour(C_TerrorMeleeWeapon* pThis, void* edx, int a2);
		}

		namespace GetMeleeWorldModel
		{
			inline Hook::CFunction Func;
			using FN = const char*(__fastcall*)(C_TerrorMeleeWeapon*, void*);
			const char* __fastcall Detour(C_TerrorMeleeWeapon* pThis, void* edx);
		}



		void Init();
	}
}
