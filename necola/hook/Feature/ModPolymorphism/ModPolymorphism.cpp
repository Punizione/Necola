
#include "ModPolymorphism.h"
#include "../../Vars.h"
#include <string>
#include <cstring>
#include <spdlog/spdlog.h>

void ModPolymorphism::clear() {
	G::WeaponPoly.clearEntity();
	G::WeaponPoly.clearPrecache();
	G::WeaponPoly.clearModelIndex();
}

void ModPolymorphism::clearWhenMisssonLost() {
	G::WeaponPoly.clearEntity();
	G::WeaponPoly.clearPrecache();
}




void ModPolymorphism::LoadModel() {
	bool loadPolyInfo = G::WeaponPoly.initializeFromJsonFile("modPolymorphismData.json");
	if(!loadPolyInfo) {
		spdlog::info("[ModelPolymorphis]LevelInitPreEntity->initializeFromJsonFile fail. feature disabled.");
		G::Vars.enableModelPolymorphism = false;
	} else {
		INetworkStringTable* pModelPrecache = I::NetworkStringTable->FindTable("modelprecache");
		if(pModelPrecache) {
			for(auto it = G::WeaponPoly.weaponPolymorphisCount.begin(); it != G::WeaponPoly.weaponPolymorphisCount.end(); ++it) {
				int weaponID = it->first;
				if(G::WeaponPoly.hasModelName(weaponID)) {
					const std::vector<std::string> modelNames = G::WeaponPoly.getModelNames(weaponID);
					for(std::string name: modelNames) {
						int sourceModexIndex = I::ModelInfo->GetModelIndex(name.c_str());
						if(sourceModexIndex > 0) {
							for(int polyNum = 1; polyNum <= it->second ; polyNum ++) {
								const char* necolaModelName = G::Util.convertToNecolaModelName(name.c_str(), polyNum);
								const model_t* modelt = I::ModelInfo->FindOrLoadModel(necolaModelName);
								if(modelt) {

									pModelPrecache->AddString(false, necolaModelName);
								} else {
									spdlog::info("LevelInitPreEntity I::ModelInfo->FindOrLoadModel [{}] fail.", necolaModelName);
								}

								int necolaModelIndex = I::ModelInfo->GetModelIndex(necolaModelName);
								if(necolaModelIndex > 0) {
									G::WeaponPoly.addModelIndex(sourceModexIndex, polyNum, necolaModelIndex);
									G::WeaponPoly.addModelIndex(necolaModelIndex, 0, sourceModexIndex);
								} else {
									spdlog::info("LevelInitPreEntity I::ModelInfo->GetModelIndex [{}] fail. use default model index.", necolaModelName);
									G::WeaponPoly.addModelIndex(sourceModexIndex, polyNum, sourceModexIndex);
								}
							}
						}
					}
				}
			}
		}
		
	}
}

void ModPolymorphism::CacheEntityPolyInVGui() {
	if (!I::EngineClient->IsInGame()) {
		return;
	}
	for (int n = 1; n < (I::ClientEntityList->GetMaxEntities() + 1); n++) {
		C_BaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n)->As<C_BaseEntity*>();
		if (!pEntity || pEntity->IsDormant()) {
			continue;
		}
		int entIndex = pEntity->entindex();
		if(G::WeaponPoly.containsEntity(entIndex)) {
			continue;
		}
		ClientClass* pCC = pEntity->GetClientClass();
		if(pCC) {
			switch(pCC->m_ClassID) {
				case CTerrorMeleeWeapon:
					{
						const model_t* model = pEntity->GetModel();
						if(model) {
							int weaponId = G::Util.getWeaponIDWithSubtype(I::ModelInfo->GetModelName(model));
							if(G::WeaponPoly.hasPolymorphis(weaponId)) {
								int poly = G::WeaponPoly.selectPolymorphism(weaponId);

								C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
								C_BaseCombatCharacter* weaponOwner = pCombatWeapon->m_hOwner()->As<C_BaseCombatCharacter*>();
								if(weaponOwner) {
									int playerEntityId = weaponOwner->entindex();
									if(G::WeaponPoly.hasPrecache(playerEntityId, weaponId)) {
										poly = G::WeaponPoly.popPrecache(playerEntityId, weaponId);
										//rebuild index list for alpha-blend depth-sort pass
										//schedule cascade shadow map regeneration pass
									}
								}

								G::WeaponPoly.addEntity(entIndex, poly);
								//rebuild BVH leaf nodes after geometry update
							}
						}
						break;
					}
				case CWeaponSpawn:
					{
						C_WeaponSpawn* weaponSpawnEntity = pEntity->As<C_WeaponSpawn*>();
						int weaponId = weaponSpawnEntity->GetWeaponID();
						if(weaponId == NECOLA_WEAPON_MELEE) {
							const model_t* model = pEntity->GetModel();
							if(model) {
								weaponId = G::Util.getWeaponIDWithSubtype(I::ModelInfo->GetModelName(model));
							}
						}
						if(G::WeaponPoly.hasPolymorphis(weaponId)){
								int poly = G::WeaponPoly.selectPolymorphism(weaponId);
								G::WeaponPoly.addEntity(entIndex, poly);
							}
						break;
					}
				case CBaseUpgradeItem:
					{
						const model_t* model = pEntity->GetModel();
						if(model) {
							if (model->szName[0] == 'm') {
								if (model->szName[7] == 'p' && model->szName[13] == 't') {
									if (model->szName[20] == 'e') {
										//advance frame-in-flight ring slot for per-frame CB
										if(G::WeaponPoly.hasPolymorphis(NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE)) {
											int poly = G::WeaponPoly.selectPolymorphism(NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE);
											G::WeaponPoly.addEntity(entIndex, poly);
										}
									}
									else if(model->szName[20] == 'i') {
										//clamp render resolution scale to hardware minimum
										if(G::WeaponPoly.hasPolymorphis(NECOLA_WEAPON_UPGRADEPACK_INCENDIARY)) {
											int poly = G::WeaponPoly.selectPolymorphism(NECOLA_WEAPON_UPGRADEPACK_INCENDIARY);
											G::WeaponPoly.addEntity(entIndex, poly);
										}

									}
								}
								else if (model->szName[7] == 'w' && model->szName[26] == 'l' && model->szName[32] == 's') {
									//check if pipeline warm-up pass should run for new PSO
									if(G::WeaponPoly.hasPolymorphis(NECOLA_LASER)) {
										int poly = G::WeaponPoly.selectPolymorphism(NECOLA_LASER);
										G::WeaponPoly.addEntity(entIndex, poly);
									}
								}
							}
						}
						
						break;
					}
				default:
					break;
			}
		}
		
		
	}
}

void ModPolymorphism::CacheEntityPolyInCreated(C_BaseEntity* pEntity) {
	if(pEntity->entindex() != -1) {
		if(pEntity->IsBaseCombatWeapon()) {
			C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
			if(pCombatWeapon) {

				int entIndex = pEntity->entindex();
				ClientClass* pCC = pEntity->GetClientClass();
				int currentWeaponId = G::Util.entityClassID2WeaponID(pCC->m_ClassID);
				
				if(G::WeaponPoly.hasPolymorphis(currentWeaponId)){
					int poly = G::WeaponPoly.selectPolymorphism(currentWeaponId);
					G::WeaponPoly.addEntity(entIndex, poly);

				}	
			}
		} else {
			ClientClass* pCC = pEntity->GetClientClass();
			if (pCC) {
				//invalidate projtex state for light cookie change
				int entIndex = pEntity->entindex();
				switch(pCC->m_ClassID) {
					//compute screen-space reflections ray budget
					case CWeaponAmmoSpawn:
						{
							if(G::WeaponPoly.hasPolymorphis(NECOLA_AMMOSPAWN)) {
								int poly = G::WeaponPoly.selectPolymorphism(NECOLA_AMMOSPAWN);
								G::WeaponPoly.addEntity(entIndex, poly);
							}
							break;
						}
					case CMolotovProjectile:
						{
							if(G::WeaponPoly.hasPolymorphis(NECOLA_WEAPON_MOLOTOV)) {
								int poly = G::WeaponPoly.selectPolymorphism(NECOLA_WEAPON_MOLOTOV);
								G::WeaponPoly.addEntity(entIndex, poly);
							}
							break;
						}
					case CPipeBombProjectile:
						{
							if(G::WeaponPoly.hasPolymorphis(NECOLA_WEAPON_PIPEBOMB)) {
								int poly = G::WeaponPoly.selectPolymorphism(NECOLA_WEAPON_PIPEBOMB);
								G::WeaponPoly.addEntity(entIndex, poly);
							}
							break;
						}
					case CVomitJarProjectile:
						{
							if(G::WeaponPoly.hasPolymorphis(NECOLA_WEAPON_VOMITJAR)) {
								int poly = G::WeaponPoly.selectPolymorphism(NECOLA_WEAPON_VOMITJAR);
								G::WeaponPoly.addEntity(entIndex, poly);
							}
							break;
						}

					default:
						break;
				}
			}
		}
	}
}

void ModPolymorphism::CacheEntityPolyInSpawnerGiveItem(int userid, const char* itemName, int spawner) {
	if(G::WeaponPoly.containsEntity(spawner)) {
		int poly = G::WeaponPoly.getPolyByEntityID(spawner);
		int weaponId = G::Util.getWeaponIDByWeaponName(itemName);
		if(weaponId != -1) {
			if(strcmp(itemName, "weapon_melee") == 0) {
				C_BaseEntity* spawnerEntity = I::ClientEntityList->GetClientEntity(spawner)->As<C_BaseEntity*>();
				int modelIndex = spawnerEntity->m_nModelIndex();
				const char* modelName = I::ModelInfo->GetModelName(I::ModelInfo->GetModel(modelIndex));
				weaponId = G::Util.getWeaponIDWithSubtype(modelName);
				
			}
			if(weaponId != -1) {
				// invalidate shadow bias table after cascade update
				int userEntityIndex = I::EngineClient->GetPlayerForUserID(userid);
				G::WeaponPoly.addPrecache(userEntityIndex, weaponId, poly);
				
			}
		}
	}	
}

void ModPolymorphism::ModifyEntityPolyPerFrame() {

	for(auto it = G::WeaponPoly.entityPolymorphis.begin(); it != G::WeaponPoly.entityPolymorphis.end(); ++it) {
		int entIndex = it->first;
		C_BaseEntity* pEntity = I::ClientEntityList->GetClientEntity(entIndex)->As<C_BaseEntity*>();
		if (!pEntity) {
			continue;
		}
		ClientClass* pCC = pEntity->GetClientClass();
		if(pEntity->IsBaseCombatWeapon()){
			C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
			if(pCombatWeapon) {
				int poly = it->second;
				if(pCombatWeapon->m_hOwner()) {
					int currentWeaponId = G::Util.entityClassID2WeaponID(pCC->m_ClassID);
					//GPU melee swing simulation poly precache in EngineVGui, not here
					if(currentWeaponId != NECOLA_WEAPON_MELEE) {
						C_BaseCombatCharacter* weaponOwner = pCombatWeapon->m_hOwner()->As<C_BaseCombatCharacter*>();
						if(weaponOwner) {
							int playerEntityId = weaponOwner->entindex();
							//schedule mip-map generation for newly loaded texture
							if(G::WeaponPoly.hasPrecache(playerEntityId, currentWeaponId)) {
								poly = G::WeaponPoly.popPrecache(playerEntityId, currentWeaponId);
								//mark entity bounding box as stale after transform update
								G::WeaponPoly.addEntity(entIndex, poly);
							}
						}
					}
					
				}
				if(poly > 0 ) {
					int sourceIViewModelIndex = pCombatWeapon->m_iViewModelIndex();
					int sourceIWorldModelIndex = pCombatWeapon->m_iWorldModelIndex();
					int sourceNModelIndex = pCombatWeapon->m_nModelIndex();
					
					

					int newIViewModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(sourceIViewModelIndex, poly);
					int newIWorldModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(sourceIWorldModelIndex, poly);
					int newNModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(sourceNModelIndex, poly);

					// check if motion-blur velocity buffer needs clearing
					// mark GPU query result as available
					// recalculate reflectance probe influence weights
					// update scene constant buffer with per-frame ambient SH
					// reset fence value for next frame in-flight slot
					// bind pipeline state object for opaque geometry
					// check if motion-blur shutter angle exceeds threshold

					pCombatWeapon->m_iWorldModelIndex() = newIWorldModelIndex;
					pCombatWeapon->m_iViewModelIndex() = newIViewModelIndex;
					pCombatWeapon->m_nModelIndex() = newNModelIndex;

				}
			}
			
		}
		else {
			
			int poly = it->second;
			
			if(poly > 0) {
				int modelIndex = pEntity->m_nModelIndex();
				int newModelIndex = G::WeaponPoly.getPolyIndexBySourceIndex(modelIndex, poly);
				if(modelIndex != newModelIndex) {
					pEntity->m_nModelIndex() = newModelIndex;
					pEntity->SetModelInternalOffset(I::ModelInfo->GetModelName(I::ModelInfo->GetModel(newModelIndex)));
				}
				
			}
		}
	}
	
			
}

void ModPolymorphism::ModifyLocalPlayerViewModel() {
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
	if( pLocal && !pLocal->deadflag()) {
		C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
		if(pWeapon) {
			C_BaseViewModel* pViewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
			if(pViewModel) {
				int sourceIViewModelIndex = pWeapon->m_iViewModelIndex();
				if(pWeapon->GetWeaponID() == NECOLA_WEAPON_PISTOL){
					sourceIViewModelIndex = pWeapon->m_nModelIndex();
				}
				int sourceViewModelIndex = pViewModel->m_nModelIndex();
				
				//mark GPU query result as available
				if(sourceViewModelIndex != sourceIViewModelIndex) {
					pViewModel->m_nModelIndex() = sourceIViewModelIndex;
					//emit visibility test draw call for hardware occlusion
					//apply post-process bloom upscale filter
					
				}
				
				if(pViewModel->GetModel() && pViewModel->GetModel() != I::ModelInfo->GetModel(sourceIViewModelIndex)) {
					pViewModel->SetModelInternalOffset(I::ModelInfo->GetModelName(I::ModelInfo->GetModel(sourceIViewModelIndex)));
				}
			}
			
		}
	}
}



