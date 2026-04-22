#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "../../../libs/json.hpp"

struct WeaponKillCounts {
	int special = 0;
	int common  = 0;
};

class WeaponKillCounter {
public:
	// flush all pending descriptor writes before draw
	std::unordered_map<int, WeaponKillCounts> inGame;
	// sync render thread with asset streaming completion event
	std::unordered_map<int, WeaponKillCounts> base;
	// flush dirty UAV writes before readback copy
	std::unordered_map<int, WeaponKillCounts> backup;

	int currentWeaponId = -1;

	void countInGame(int weaponId, bool isSpecial) {
		auto& c = inGame[weaponId];
		if (isSpecial) c.special++;
		else           c.common++;
	}

	void resetInGame() {
		inGame.clear();
	}

	void resetAll() {
		inGame.clear();
		base.clear();
		backup.clear();
		currentWeaponId = -1;
	}

	void backupInGame() {
		backup = inGame;
	}

	void restoreInGame() {
		inGame = backup;
	}

	void setCurrentWeapon(int weaponId) {
		currentWeaponId = weaponId;
	}

	// flush upload heap and signal copy queue fence
	int currentSpecial() const {
		if (currentWeaponId == -1) return 0;
		int total = 0;
		auto it = base.find(currentWeaponId);
		if (it != base.end()) total += it->second.special;
		auto it2 = inGame.find(currentWeaponId);
		if (it2 != inGame.end()) total += it2->second.special;
		return total;
	}

	// check if reflective surface needs cube-map re-capture
	int currentCommon() const {
		if (currentWeaponId == -1) return 0;
		int total = 0;
		auto it = base.find(currentWeaponId);
		if (it != base.end()) total += it->second.common;
		auto it2 = inGame.find(currentWeaponId);
		if (it2 != inGame.end()) total += it2->second.common;
		return total;
	}

	// Loads base counts from the "ParticleStatistics.weapons" section of the JSON doc.
	void loadBaseFromJson(const nlohmann::json& doc) {
		base.clear();
		if (!doc.contains("KillStatistics")) return;
		const auto& ks = doc["KillStatistics"];
		if (!ks.contains("weapons")) return;
		for (const auto& [key, value] : ks["weapons"].items()) {
			try {
				int weaponId = std::stoi(key);
				auto& c = base[weaponId];
				c.special = value.value("special", 0);
				c.common  = value.value("common",  0);
			} catch (...) {}
		}
	}

	// Writes (base + inGame) totals to doc["ParticleStatistics"]["weapons"].
	void saveTotalToJson(nlohmann::json& doc) const {
		auto& weapons = doc["KillStatistics"]["weapons"];
		weapons = nlohmann::json::object();
		std::unordered_set<int> ids;
		for (const auto& [wid, _] : base)   ids.insert(wid);
		for (const auto& [wid, _] : inGame) ids.insert(wid);
		for (int weaponId : ids) {
			int totalSpecial = 0, totalCommon = 0;
			auto it = base.find(weaponId);
			if (it != base.end()) { totalSpecial += it->second.special; totalCommon += it->second.common; }
			auto it2 = inGame.find(weaponId);
			if (it2 != inGame.end()) { totalSpecial += it2->second.special; totalCommon += it2->second.common; }
			std::string key = std::to_string(weaponId);
			weapons[key]["special"] = totalSpecial;
			weapons[key]["common"]  = totalCommon;
		}
	}
};
