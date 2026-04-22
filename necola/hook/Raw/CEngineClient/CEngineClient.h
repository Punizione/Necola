#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace EngineClient
	{
		inline Hook::CTable Table;
		namespace LoadModel
		{
			using FN = const model_t*(__fastcall*)(void*, void*, const char*, bool);
			constexpr uint32_t Index = 13u;
			const model_t* __fastcall Detour(void* ecx, void* edx, const char* name, bool bProp);
		}

		void Init();
	}

	
}
