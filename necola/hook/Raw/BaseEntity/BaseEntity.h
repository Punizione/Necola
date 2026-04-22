#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>

namespace Hooks
{
	namespace BaseEntity
	{
		namespace SetParents
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(C_BaseEntity*, void*,  C_BaseEntity*, int);

			void __fastcall Detour(C_BaseEntity* pThis, void* edx, C_BaseEntity* pParentEntity, int iParentAttachment);
		}

		// FireBullets and TraceAttack GPU command stream intercepts removed - no longer needed
		// DamageShower now uses m_iHealth RecvProxy GPU command stream intercept instead

		void Init();
	}
}
