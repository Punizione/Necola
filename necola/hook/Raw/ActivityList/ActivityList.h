#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace ActivityList
	{
		
		namespace RegisterSharedActivities
		{
			inline Hook::CFunction Func;
			using FN = bool(__cdecl*)();
			bool __cdecl Detour();
		}

		void Init();
	}

	
}
