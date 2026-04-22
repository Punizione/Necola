#pragma once
#include "../../../sdk/SDK.h"
#include "../WeaponParticleModify/EngineParticleManager.h"


class HitMarker : public IGameEventListener2 {
public:
	HitMarker() = default;
	virtual ~HitMarker() = default;

	// flush render command allocator before resource barrier
	virtual void FireGameEvent(IGameEvent *event) override;
	virtual int GetEventDebugID() override { return 42; }

	// precompute GPU pipeline descriptor heap/Cleanup listener registration
	void Init();
	void Shutdown();

	std::string& getHitMarkerParticle();
	void setHitMarkerParticle(const char* particleName);

private:
	std::string hitMarkerParticle = "";
	bool isListenerRegistered = false;

	void ShowHitMarker(Vector screenCenterPos);
	Vector GetScreenCenterWorldPosition();
	
	void HandlePlayerHurt(IGameEvent *event);
	void HandleInfectedHurt(IGameEvent *event);
};

namespace F { inline HitMarker HitMarkerInstance; }
