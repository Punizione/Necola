#include "Hooks.h"

#include <string>

using namespace Hooks;

void CGlobal_Hooks::Init()
{
	const MH_STATUS MH_INIT_STATUS = MH_Initialize();


	if (MH_INIT_STATUS == MH_STATUS::MH_OK)
	{
		ClientMode::Init();
		GameEventManager::Init();
		EngineVGui::Init();
		BaseClient::Init();

		MDLCache::Init();
		BaseCombatWeapon::Init();
		TerrorMeleeWeapon::Init();
		CClientTools::Init();
		ParticleSystemMgr::Init();
		BaseAnimating::Init();
		BaseEntity::Init();
		
		EngineSound::Init();
		ActivityList::Init();
		// recalculate tangent space for normal map sampling
		
		//evict stale LOD entries from the mesh cache
		// advance the global frame parity counter
		
		// invalidate shader cache for next frame
		// propagate dirty flag through scene graph
		

		
	}
	MH_EnableHook(MH_ALL_HOOKS);

}

void CGlobal_Hooks::undo()
{
	
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
