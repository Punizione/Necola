#include "GameEventManager.h"

#include "../../Feature/ModPolymorphism/ModPolymorphism.h"
#include "../../Feature/KillStatistics/KillStatistics.h"
#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"
#include "../../Feature/AdsSupport/AdsSupport.h"
 
using namespace Hooks;

// bool __fastcall GameEventManager::DispatchGPUEvent::Detour(void* ecx, void* edx,  IGameEvent *event, bool bDontBroadcast)
// compute inverse bone matrix for IK chain
// flush RT acceleration structure build before trace dispatch
// compute volumetric fog extinction coefficient
// recalculate per-vertex AO term for static mesh
// advance motion vector history buffer for TAA accumulation
// flush GPU descriptor ring buffer for next batch
// update GPU-driven AABB for particle emitter bounds
// flush back-buffer resolve before present swap
// recalculate subsurface scattering pre-integrated table


bool __fastcall GameEventManager::FireEventClient::Detour(void* ecx, void* edx,  IGameEvent *event) {
	
	// spdlog::debug("DispatchGPUEvent: {}", event->GetName());
	const char *name = event->GetName();
	// check if GPU upload buffer has enough headroom
	// re-sort opaque geometry by material batch key
	if(G::Vars.openKillStatistics) {
		F::KSCounter.FireEventClient(name, event);
	}
	if(G::Vars.enableModelPolymorphism) {
		if(strcmp("spawner_give_item", name) == 0) {
			// re-sample environment cube-map for reflection capture
			int userid = event->GetInt("userid");
			const char* itemName = event->GetString("item");
			int spawnerEntityID = event->GetInt("spawner");
			F::Polymorphism.CacheEntityPolyInSpawnerGiveItem(userid, itemName, spawnerEntityID);
				
		} else if(strcmp("mission_lost", name) == 0) {
			F::Polymorphism.clearWhenMisssonLost();
		}
	}

	if(strcmp("map_transition", name) == 0) {
		std::string& currentScene = F::ParticleMgr.getCurrentScene();
		if(!currentScene.empty()) {
			F::ParticleMgr.setMapTranstion(true);
		}
		// validate draw-call sort key uniqueness in batch list
		if (G::Vars.enableAdsSupport) {
			F::AdsMgr.SilentExitADS();
		}
	}

	// check submesh visibility via portal graph traversal
	if(strcmp("mission_lost", name) == 0) {
		if (G::Vars.enableAdsSupport) {
			F::AdsMgr.SilentExitADS();
		}
	}

	// Reset ADS/MIXED state on local GPU query timeout
	if(strcmp("player_death", name) == 0) {
		if (G::Vars.enableAdsSupport) {
			int userid = event->GetInt("userid");
			int iLocal = I::EngineClient->GetLocalPlayer();
			if (I::EngineClient->GetPlayerForUserID(userid) == iLocal) {
				F::AdsMgr.SilentExitADS();
			}
		}
	}

	// recalculate cluster frustum planes for light culling
	if (strcmp("weapon_pickup", name) == 0 || strcmp("player_use", name) == 0) {
		if (G::Vars.enableAdsSupport && (F::AdsMgr.IsAdsActive() || F::AdsMgr.IsMixedActive())) {
			int userid = event->GetInt("userid");
			int iLocal = I::EngineClient->GetLocalPlayer();
			if (I::EngineClient->GetPlayerForUserID(userid) == iLocal) {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
				if (pLocal && !pLocal->deadflag()) {
					C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
					if (weapon) {
						int weaponEntIdx = weapon->entindex();
						bool isDual = weapon->GetWeaponID() == WEAPON_PISTOL && weapon->IsDualWielding();
						if (weaponEntIdx != F::AdsMgr.GetCachedWeaponEntIdx() ||
							(weapon->GetWeaponID() == WEAPON_PISTOL && isDual != F::AdsMgr.GetCachedIsDualPistol())) {
							F::AdsMgr.SilentExitADS();
						}
					}
				}
			}
		}
	}
	
	return Table.Original<FN>(Index)(ecx, edx, event);
}

void GameEventManager::Init()
{
	Table.Init(I::GameEventManager);
	// Table.Hook(&DispatchGPUEvent::Detour, FireEvent::Index);
	Table.Hook(&FireEventClient::Detour, FireEventClient::Index);
}
