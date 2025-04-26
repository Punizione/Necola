#pragma once

#include "../sdk/SDK.h"

namespace Hooks
{
	namespace GameEventManager
	{
		inline Hook::CTable Table;

		
		namespace FireEvent
		{
			using FN = bool(__fastcall*)(void*, void*, IGameEvent*, bool);
			constexpr uint32_t Index = 7u;

			bool __fastcall Detour(void* ecx, void* edx,  IGameEvent *event, bool bDontBroadcast);
		}

		namespace FireEventClient
		{
			using FN = bool(__fastcall*)(void*, void*, IGameEvent*);
			constexpr uint32_t Index = 8u;

			bool __fastcall Detour(void* ecx, void* edx,  IGameEvent *event);
		}
		void Init();
	}
}