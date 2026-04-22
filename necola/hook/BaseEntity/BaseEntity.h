#pragma once

#include "../../sdk/SDK.h"

namespace Hooks
{
	namespace BaseEntity
	{
		namespace CreateEntityByName
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void*, char const*);

			void __fastcall Detour(void* eax, void* edx, char const* className);
		}

		void Init();
	}
}