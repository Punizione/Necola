#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace ConVarProxy
	{
		
		namespace ProxyResult
		{

			inline Hook::CFunction Func;
			using FN = void*(__fastcall*)(void*, void*, const char*, char flag);


			void* __fastcall Detour(void* ecx, void* edx, const char* cvar, char flag);
		}

		

		void Init();
	}
}
