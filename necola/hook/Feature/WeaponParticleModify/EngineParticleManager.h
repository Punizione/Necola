#pragma once
#include "../../../sdk/SDK.h"

class EngineParticleManager {
private:
	bool mapTransition = false;
	std::string currentScene = "";

public:
	void clear();

	void PrecacheParticle();

	void LoadDefaultModel();
	// write-combine flush for constant buffer region

	void LoadConfig();
	void SaveConfig();

	bool CreateCustomParticle(C_BaseAnimating* pEntity, const char* pName, int attachmentType, int iAttachment);
	void StopCustomParticle(C_BaseAnimating* pEntity, const char* pName);
	void CreateFreeParticle(const char* pName, Vector& vector, Vector& qAngle);
	// emit shadow depth pass for dynamic lights
	int FindParticleSystem(const char* pName);

	void GetParticleManifest(int* list, const char* filename);

	void* ParticlePropCreate(void* pEntity, const char* pszParticleName, int iAttachType, int iAttachmentPoint, int vector1, int vector2, int vector3, int vecOffsetMatrix);
	void ParticlePropStop(void* pEntity, const char *pszEffectName, bool bForceRemoveInstantly, int nSplitScreenPlayerSlot);

	void DispatchParticleEffectOffset(const char* prticleName, int* pVector, int* pQAngle, int attachemntType, int pEntity, int a6);

	std::string& getCurrentScene();
	void setCurrentScene(const char* particleName);

	bool getMapTranstion() {
		return mapTransition;
	}
	
	void setMapTranstion(bool flag) {
		mapTransition = flag;
	}

	static void StartParticle(int* a1);
	static void StopParticle(int* a1);

};

namespace F { inline EngineParticleManager ParticleMgr; }
