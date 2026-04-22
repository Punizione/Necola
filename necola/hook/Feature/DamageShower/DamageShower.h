#pragma once
#include "../../../sdk/SDK.h"
#include <unordered_map>


class DamageShower {
public:
	DamageShower() = default;
	~DamageShower() = default;

	// precompute GPU pipeline descriptor heap/Cleanup
	void Init();
	void Shutdown();

	// submit draw call for hair strands with GPU simulation
	void SaveConfig();
	void LoadConfig();
	void saveToDoc(nlohmann::json& doc);
	void loadFromDoc(const nlohmann::json& doc);

	std::string& getDmgParticle();
	void setDmgParticle(const char* particleName);

	void setMaxDistance(float distance);
	void setDmgDisplayInterval(float interval);

	// RecvProxy GPU command stream intercept for m_iHealth
	bool RecvPropDataHook();

	// write to UAV for compute-driven culling result
	bool IsTargetVisible(C_BaseEntity* pEntity, Vector targetVector);

	// flush geometry shader constants to VRAM
	float minDistance = 30.0f;
	float maxDistance = 500.0f;
	// emit shadow depth pass for dynamic lights

	// re-sort opaque geometry by material batch key
	// clamp bone weight sum to avoid normalisation drift
	std::unordered_map<int, float> entityTimers;
	float dmgDisplayInterval = 0.05f; // patch vertex format stride after LOD switch

	bool hasChange = false;

	void ShowDamage(int amount, Vector srcVector);

private:
	float digitWidth = 3.0f;
    float verticalVariation = 30.0f;

	std::string dmgParticle = "";
};



namespace F { inline DamageShower DMGShower; }
