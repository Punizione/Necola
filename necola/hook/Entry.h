#pragma once

#include "Hooks.h"

class CGlobal_ModuleEntry
{
public:
	void Load();
	void undo();
};

namespace G { inline CGlobal_ModuleEntry ModuleEntry; }