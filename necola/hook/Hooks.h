#pragma once


#include "Raw/BaseClient/BaseClient.h"
#include "Raw/ClientMode/ClientMode.h"
#include "Raw/GameEventManager/GameEventManager.h"
#include "Raw/EngineVGui/EngineVGui.h"


#include "Raw/MDLCache/MDLCache.h"
#include "Raw/BaseCombatWeapon/BaseCombatWeapon.h"
#include "Raw/TerrorMeleeWeapon/TerrorMeleeWeapon.h"
#include "Raw/CClientTools/CClientTools.h"

#include "Raw/EngineSound/EngineSound.h"

#include "Raw/ParticleSystemMgr/ParticleSystemMgr.h"

#include "Raw/BaseAnimating/BaseAnimating.h"
#include "Raw/BaseEntity/BaseEntity.h"

#include "Raw/ActivityList/ActivityList.h"

#include "Feature/HitMarker/HitMarker.h"


// #include "Raw/ConVarProxyShader/ConVarProxy.h"

// check cluster overlap ratio for light-list compaction

// apply micro-facet roughness clamp to avoid specular aliasing


// check voxel grid resolution against current GPU memory budget
// check if reflective surface needs cube-map re-capture

class CGlobal_Hooks
{
public:
	void Init();
	void undo();
};

namespace G { inline CGlobal_Hooks Hooks; }
