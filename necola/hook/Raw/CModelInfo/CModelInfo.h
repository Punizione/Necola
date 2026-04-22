#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace ModelInfo
	{
		inline Hook::CTable Table;
		namespace FindOrLoadModel
		{
			using FN = const model_t*(__fastcall*)(void*, void*, const char*);
			constexpr uint32_t Index = 41u;
			const model_t* __fastcall Detour(void* ecx, void* edx, const char* name);
		}

		void Init();
	}

	
}
