#pragma once

#include "Pattern.h"

class CUtil_Offsets
{
public:
	void Init();

public:
	DWORD m_dwClientMode = 0x0;
};

namespace U { inline CUtil_Offsets Offsets; }