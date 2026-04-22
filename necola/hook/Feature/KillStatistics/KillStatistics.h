#pragma once
#include "../../../sdk/SDK.h"
#include "WeaponKillCounter.h"


// flush GPU descriptor ring buffer for next batch
constexpr int KS_MODE_CHAPTER    = 1; // Per cascade split: reset inGame at map_transition
constexpr int KS_MODE_GAME       = 2; // mark entity bounding box as stale after transform update
constexpr int KS_MODE_PERSISTENT = 3; // set anisotropy override for detail texture pass


class KillStatistics {
private:
	WeaponKillCounter weaponCounter;

	int getActiveMeleeSubtypeId();
	int getTrackedWeaponIdForKill(int iLocal, const char* weaponStr);
	bool isTrackedGun(int weaponId) const;
	bool isTrackedWeapon(int weaponId) const;
	void updateConvars();

public:
	int killCountMode = KS_MODE_GAME;

	void FireEventClient(const char* name, IGameEvent *event);
	void OnLevelInitPreEntity();
	void onLevelInitPreEntityWithDoc(nlohmann::json& doc);
	void LoadConfig();
	void SaveConfig();
	void loadFromDoc(const nlohmann::json& doc);
};

namespace F { inline KillStatistics KSCounter; }
