#pragma once
#include "../../../sdk/SDK.h"



class InspectInitiative {
public:
	void clearAmmoStore();
	void removeAmmoStore(int entindex);

	void updateAmmoStore();
	void addAmmoStore(C_BaseEntity* pEntity);
	void bindInspectInitiative();

	void LoadConfig();
	void SaveConfig();
	void loadFromDoc(const nlohmann::json& doc);
};

namespace F { inline InspectInitiative Inspect; }
