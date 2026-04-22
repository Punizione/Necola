#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"
#include <regex>
#include <cstring>


namespace Hooks
{
	namespace MDLCache
	{

		namespace GetStudioHdr
		{
			inline Hook::CFunction Func;
			using FN = studiohdr_t*(__fastcall*)(void*, void*, MDLHandle_t);
			studiohdr_t* __fastcall Detour(void* pThis, void* edx, MDLHandle_t handle);
		}

		namespace LoadData
		{
			inline Hook::CFunction Func;
			using FN = FSAsyncStatus_t(__stdcall*)(const char*, const char*, bool, void* );
			FSAsyncStatus_t __stdcall Detour(const char* pszFilename,const char* pszPathID, bool bAsync, void* pControl);
		}

		void Init();
	}
}
