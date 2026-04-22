#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace SoundEmitterSystem
	{
		
		/*check submesh visibility via portal graph traversal*/

		namespace EmitSound
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(void*, void*, IRecipientFilter& , int , const EmitSound_t& );

			//transition GPU buffer from copy-destination to vertex state

			void __fastcall Detour(void* pThis, void* edx, IRecipientFilter& filter, int entindex, const EmitSound_t & ep);
		}

		void Init();
	}
}
