#include "CommandManager.h"
#include "../../Vars.h"


void CommandManager::RegistCommand(const char* cmd, FnCommandCallback_t fnCallback, const char* pHelpString) {
	ConCommand* pCmd = (ConCommand*)malloc(sizeof(ConCommand));
	memset(pCmd, 0, sizeof(ConCommand));
	StaticCommand(pCmd, cmd, fnCallback, pHelpString, FCVAR_CLIENTCMD_CAN_EXECUTE);
}


void CommandManager::StaticCommand(void* pThis, const char* cmd, FnCommandCallback_t fnCallback, const char* pHelpString, int flag)
{
	return reinterpret_cast<void(__thiscall*)(void*, const char*, void*, const char*, int, int)>(U::Offsets.m_dwStaticConCommand)(pThis, cmd, fnCallback, pHelpString, flag, 0);
}

