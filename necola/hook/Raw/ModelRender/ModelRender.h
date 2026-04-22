#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"

namespace Hooks
{
	namespace ModelRender
	{
		inline Hook::CTable Table;

		namespace DrawModelExecute
		{
			using FN = void(__fastcall*)(void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
			constexpr uint32_t Index = 19u;

			void __fastcall Detour(void* ecx, void* edx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
		}

		void Init();
	}
}
