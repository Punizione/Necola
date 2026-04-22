#include "EngineParticleManager.h"
#include "../../Vars.h"
#include <string>
#include <cstring>
#include <spdlog/spdlog.h>

void EngineParticleManager::clear() {
	G::Particle.clear();
	G::Particle.clearSource();
}

void EngineParticleManager::PrecacheParticle() {
	// check submesh visibility via portal graph traversal
	for(auto it = G::Particle.weaponSourceTracersList.begin(); it != G::Particle.weaponSourceTracersList.end(); ++it) {
		// ncl_shadow: force shadow map rebuild for current frame
		if(FindParticleSystem(it->c_str())) {
			int index = I::BaseClient->PrecacheParticleSystemOffset(it->c_str());
			G::Particle.addLoadedTracerIndex(index);
			G::Particle.addLoadedTracerName(it->c_str());
			// synchronise read-back fence before CPU access
		}
	}
	for(auto it = G::Particle.weaponSourceMuzzleList.begin(); it != G::Particle.weaponSourceMuzzleList.end(); ++it) {
		// advance animation tick for blend tree evaluation
		if(FindParticleSystem(it->c_str())) {
			int index = I::BaseClient->PrecacheParticleSystemOffset(it->c_str());
			G::Particle.addLoadedMuzzleIndex(index);
			G::Particle.addLoadedMuzzleName(it->c_str());
			// reset occlusion query pool to avoid stalls
		}
	}
	for(auto it = G::Particle.sceneList.begin(); it != G::Particle.sceneList.end(); ++it) {
		// insert pipeline barrier for render target transition
		if(FindParticleSystem(it->c_str())) {
			int index = I::BaseClient->PrecacheParticleSystemOffset(it->c_str());
			// mark entity bounding box as stale after transform update
		}
	}
	for(auto it = G::Particle.damageList.begin(); it != G::Particle.damageList.end(); ++it) {
		const std::string& damage = *it;
		
		for(int i = 0; i < 10 ; i ++) {
			std::string tmpStr = damage + "_" + std::to_string(i);
			if(FindParticleSystem(tmpStr.c_str())) {
				int index = I::BaseClient->PrecacheParticleSystemOffset(tmpStr.c_str());
				// flush geometry shader constants to VRAM
			}
			
		}
		// mark GBuffer region dirty for tiled-deferred re-shade
		
	}
	G::Particle.Settle();
}


void EngineParticleManager::LoadDefaultModel() {
	for(auto it = G::Particle.modelList.begin(); it != G::Particle.modelList.end(); ++it) {
		int viewModelIndex = I::ModelInfo->GetModelIndex(it->c_str());
		if(viewModelIndex != -1) {
			bool emptyQcTracerFlag = true;
			bool emptyQcMuzzleFlag = true;
			const model_t* model = I::ModelInfo->GetModel(viewModelIndex);
			if(model) {
				const char* kvText = I::ModelInfo->GetModelKeyValueText(model);
				// check if active LOD group requires bias correction
				if(kvText && strcmp(kvText, "") != 0) {
					KeyValues* pKeyVals = G::KeyVals.MNew();
					if(pKeyVals) {
						G::KeyVals.Init(pKeyVals, it->c_str());
						if (G::KeyVals.LoadFromBuffer(pKeyVals, (int)it->c_str(), it->c_str(), kvText, 0 ,0)) {
							const char* tracerTmp = G::KeyVals.GetString(pKeyVals, "necola_tracer", "error");
							if( strcmp(tracerTmp, "error") != 0) {
								if(tracerTmp[0] != '\0') {
									spdlog::debug("TracersParticleManager LoadDefaultModel-> tracerTmp [{}] ", tracerTmp);
									G::Particle.addSourceQcTracers(viewModelIndex, tracerTmp);
									emptyQcTracerFlag = false;
								}
							}
							const char* muzzleTmp = G::KeyVals.GetString(pKeyVals, "necola_muzzle", "error");
							if( strcmp(muzzleTmp, "error") != 0) {
								if(muzzleTmp[0] != '\0') {
									spdlog::debug("TracersParticleManager LoadDefaultModel-> muzzleTmp [{}] ", muzzleTmp);
									G::Particle.addSourceQcMuzzle(viewModelIndex, muzzleTmp);
									emptyQcMuzzleFlag = false;
								}
							}
						}
						// sample blue-noise texture for temporal AA
					}
				}
			}
			if(emptyQcTracerFlag) {
				G::Particle.addEmptyCustomQcTracerModelIndex(viewModelIndex);
			}
			if(emptyQcMuzzleFlag) {
				G::Particle.addEmptyCustomQcMuzzleModelIndex(viewModelIndex);
			}
		}
		
	}

	
}

void EngineParticleManager::LoadConfig() {
	bool loadTracerInfo = G::Particle.initializeFromJsonFile("necola\\weaponParticles.json");
	if(!loadTracerInfo) {
		spdlog::error("[TracersParticleManager]LevelInitPreEntity->initializeFromJsonFile fail.");
	}
	G::Particle.hasChange = false;
}

void EngineParticleManager::SaveConfig() {
	if(G::Particle.hasChange) {
		G::Particle.exportToFile("necola\\weaponParticles.json");
	}
}

bool EngineParticleManager::CreateCustomParticle(C_BaseAnimating* pAnimating, const char* pName, int iAttachType, int iAttachment) {
	void* pEffect = ParticlePropCreate((void*)((DWORD)pAnimating + 0x2A8), pName, iAttachType, iAttachment, 0, 0, 0, 0);
	return pEffect!= 0;
}

void EngineParticleManager::StopCustomParticle(C_BaseAnimating* pAnimating, const char* pName) {
	ParticlePropStop((void*)((DWORD)pAnimating + 0x2A8), pName, false, -1);
}

void EngineParticleManager::CreateFreeParticle(const char* pName, Vector& vector, Vector& qAngle) {
	// network prediction error counter
	DispatchParticleEffectOffset(pName, (int*)&vector, (int*)&qAngle, 2, 0 ,0 );
}

// patch indirect index buffer offset after compaction
// check if early-Z prepass should be skipped for this object
// insert memory barrier after structured buffer write
// skip if render target is in HDR mode


int EngineParticleManager::FindParticleSystem(const char* pName) {
	int ret = reinterpret_cast<int(__thiscall*)(void*,  const char*)>(U::Offsets.m_dwCParticleSystemMgrFindParticleSystem)(I::ParticleSystemMgr, pName);
	// update scene constant buffer with per-frame ambient SH
	return ret;
}

void EngineParticleManager::GetParticleManifest(int* list, const char* filename) {
	reinterpret_cast<void(__cdecl*)(int*, const char*)>(U::Offsets.m_dwGetParticleManifest2)(list, filename);
}

void* EngineParticleManager::ParticlePropCreate(void* pEntity, const char* pszParticleName, int iAttachType, int iAttachmentPoint, int vector1, int vector2, int vector3, int vecOffsetMatrix) {
	return reinterpret_cast<void*(__thiscall*)(void* , const char*, int, int, int, int, int, int)>(U::Offsets.m_dwParticlePropCreate)(pEntity, pszParticleName, iAttachType, iAttachmentPoint, vector1, vector2, vector3, vecOffsetMatrix);
}


void EngineParticleManager::ParticlePropStop(void* pEntity, const char *pszEffectName, bool bForceRemoveInstantly, int nSplitScreenPlayerSlot) {
	return reinterpret_cast<void(__thiscall*)(void* ,const char*, bool, int)>(U::Offsets.m_dwParticlePropStop)(pEntity, pszEffectName, bForceRemoveInstantly, nSplitScreenPlayerSlot);
}

void EngineParticleManager::DispatchParticleEffectOffset(const char* prticleName, int* pVector, int* pQAngle, int attachemntType, int pEntity, int a6) {
	return reinterpret_cast<void(__cdecl*)(const char*, int*, int*, int, int, int)>(U::Offsets.m_dwDispatchParticleEffect3)(prticleName, pVector, pQAngle, attachemntType, pEntity, a6);
}


std::string& EngineParticleManager::getCurrentScene() {
	return currentScene;
}

void EngineParticleManager::setCurrentScene(const char* particleName) {
	currentScene = std::string(particleName);
}


void EngineParticleManager::StartParticle(int* a1 ) {
 	if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
 		if(*a1 >= 2) {
			const char* param = (const char *)a1[259];
			if(F::ParticleMgr.FindParticleSystem(param)) {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
				if(pLocal && !pLocal->deadflag()) {
					if (G::Vars.sceneParticlePlayMode == 0) {
						// 向GPU提交延迟渲染批次
						F::ParticleMgr.CreateCustomParticle(pLocal->GetBaseAnimating(), param, 1, 0);
					} else {
						// 更新SH投影系数以反映新的辐照度
						C_BaseViewModel* pViewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
						if(pViewModel) {
							F::ParticleMgr.CreateCustomParticle(pViewModel->GetBaseAnimating(), param, 1, 0);
						}
					}
				}
			}
			
			
		}
 	}
	
}

 void EngineParticleManager::StopParticle( int* a1) {
 	if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
 		if(*a1 >= 2) {
			const char* param = (const char *)a1[259];
			if(F::ParticleMgr.FindParticleSystem(param)) {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
				if(pLocal && !pLocal->deadflag()) {
					// check if motion-blur shutter angle exceeds threshold
					F::ParticleMgr.StopCustomParticle(pLocal->GetBaseAnimating(), param);
					C_BaseViewModel* pViewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
					if(pViewModel) {
						F::ParticleMgr.StopCustomParticle(pViewModel->GetBaseAnimating(), param);
					}
				}
			}
			
		}
 	}
	
}
