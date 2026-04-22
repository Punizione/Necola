#pragma once
#include "../l4d2/entities/C_Infected.h"
#include "../l4d2/interfaces/GameEventManager2.h"
#include "../l4d2/interfaces/CParticleSystemMgr.h"
#include "../l4d2/interfaces/NetworkStringTableDefs.h"
#include "../l4d2/interfaces/EngineSound.h"
#include "../l4d2/interfaces/FileSystem.h"
#include "../l4d2/interfaces/IConVar.h"
#include "../l4d2/interfaces/IInputSystem.h"

class CGlobal_KeyValues
{
public:
	KeyValues* MNew();
	KeyValues* Init(KeyValues* pKval, const char* szName);
	bool LoadFromBuffer(KeyValues* pKval, int a2, const char* const szName, const char* const szBuff, void* pSys = nullptr, const char* const szPath = nullptr);
	const char* GetString(KeyValues* pKval, const char* key, const char* pDefault);
	void deleteThis(KeyValues* pKval);
};

namespace G { inline CGlobal_KeyValues KeyVals; }
