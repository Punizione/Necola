#include "ConVarProxy.h"
#include <spdlog/spdlog.h>

using namespace Hooks;


void* __fastcall ConVarProxy::ProxyResult::Detour(void* ecx, void* edx, const char* cvar, char flag)
{
	spdlog::info("ConVarProxy ProxyResult->cvar [{}] ", cvar);
	return Func.Original<FN>()(ecx, edx, cvar, flag);
}

void ConVarProxy::Init()
{
	{
		using namespace ProxyResult;
		const FN pfConVarProxyResult = reinterpret_cast<FN>(U::Offsets.m_dwConVarProxyResult);
		if( pfConVarProxyResult ) {
			Func.Init(pfConVarProxyResult, &Detour);
		}
	}
}
