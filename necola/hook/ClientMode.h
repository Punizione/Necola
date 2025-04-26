#pragma once

#include "../sdk/SDK.h"

namespace Hooks
{
	namespace ClientMode
	{
		inline Hook::CTable Table;

		
		namespace CreateMove
		{
			using FN = bool(__fastcall*)(void*, void*, float, CUserCmd*);
			constexpr uint32_t Index = 27u;

			bool __fastcall Detour(void* ecx, void* edx, float input_sample_frametime, CUserCmd* cmd);
		}

		
		void Init();
	}
}