#pragma once
#include "../libs/json.hpp"

#include "utils/GameUtil.h"
#include "utils/WeaponModelsManager.h"
#include "utils/EntityMaxAmmoStore.h"
#include "utils/CustomParticleManager.h"
#include "utils/DefaultViewModelSequence.h"

namespace I { 
	inline void* ClientMode = nullptr; 
}

struct WeaponSpawnInfo_t
{
	const wchar_t* m_szName;
};

