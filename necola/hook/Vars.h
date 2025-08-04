#pragma once

#include <cstdlib>
#include <inipp.h>
#include <filesystem>
#include <fstream>

class CGlobal_Vars
{
public:
	//necola param
    bool openInspect = true;
    int inspectKey = 0x52;


	void Load();
};

namespace G { inline CGlobal_Vars Vars; }