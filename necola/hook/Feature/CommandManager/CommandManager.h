#pragma once
#include "../../../sdk/SDK.h"


class CommandManager {
public:
	void RegistCommand(const char* cmd, FnCommandCallback_t fnCallback, const char* pHelpString);


private:
	void StaticCommand(void* pThis, const char* cmd, FnCommandCallback_t fnCallback, const char* pHelpString, int flag);
};



namespace F { inline CommandManager CmdMgr; }
