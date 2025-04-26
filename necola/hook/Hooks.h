#pragma once

#include "ClientMode.h"
#include "GameEventManager.h"

class CGlobal_Hooks
{
public:
	void Init();
	void undo();
};

namespace G { inline CGlobal_Hooks Hooks; }