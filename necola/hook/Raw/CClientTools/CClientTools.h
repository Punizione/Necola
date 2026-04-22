#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>

namespace Hooks
{
	namespace CClientTools
	{
		namespace OnEntityCreated
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void* , C_BaseEntity*);
			void __fastcall Detour(void* ecx, void* edx, C_BaseEntity* pEntity);
		}

		namespace OnEntityDeleted
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void* , C_BaseEntity*);
			void __fastcall Detour(void* ecx, void* edx, C_BaseEntity* pEntity);
		}

		void Init();
	}
}
