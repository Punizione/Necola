#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>

namespace Hooks
{
	namespace BaseClient
	{
		inline Hook::CTable Table;
		namespace LevelInitPreEntity
		{
			using FN = void(__fastcall*)(void*, void*, char const*);
			constexpr uint32_t Index = 4u;

			void __fastcall Detour(void* ecx, void* edx, char const* pMapName);
		}

		namespace LevelInitPostEntity
		{
			using FN = void(__fastcall*)(void*, void*);
			constexpr uint32_t Index = 5u;

			void __fastcall Detour(void* ecx, void* edx);
		}

		namespace LevelShutdown
		{
			using FN = void(__fastcall*)(void*, void*);
			constexpr uint32_t Index = 6u;

			void __fastcall Detour(void* ecx, void* edx);
		}
		

		namespace FrameStageNotify
		{
			using FN = void(__fastcall*)(void*, void*, ClientFrameStage_t);
			constexpr unsigned int Index = 34u;

			void __fastcall Detour(void* ecx, void* edx, ClientFrameStage_t curStage);
		}


		namespace IN_KeyEvent
		{
			using FN = int(__fastcall*)(void*, void*, int, int, const char*);
			constexpr unsigned int Index = 19u;

			int __fastcall Detour(void* ecx, void* edx, int eventcode, int keynum, const char* pszCurrentBinding);
		}

		void Init();
	}

	
}
