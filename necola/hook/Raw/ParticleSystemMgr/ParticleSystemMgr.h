#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"


namespace Hooks
{
	namespace ParticleSystemMgr
	{
		namespace PrecacheParticleSystem
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void*, int, const char*);

			void __fastcall Detour(void* pThis, void* edx, int nIndex, const char* pParticleSystemName);
		}

		namespace FindPrecachedParticleSystem
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(void*, void*, int );

			int __fastcall Detour(void* pThis, void* edx, int nPrecacheIndex);
		}

		namespace GetParticleManifest
		{
			inline Hook::CFunction Func;
			using FN = void(__cdecl*)(int*);
			void __cdecl Detour(int* list);
		}

		void Init();
	}
}
