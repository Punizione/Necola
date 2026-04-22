#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"

namespace Hooks
{
	namespace ModelRenderSystem
	{
		namespace DrawModels
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void*, ModelRenderSystemData_t*, int, int);

			void __fastcall Detour(void* ecx, void* edx, ModelRenderSystemData_t* pEntities, int nCount, int renderMode);
		}

		void Init();
	}
}
