#pragma once
#include "../../../sdk/SDK.h"


class ModPolymorphism {
public:
	void clear();
	void clearWhenMisssonLost();

	void LoadModel();
	void CacheEntityPolyInVGui();
	void CacheEntityPolyInCreated(C_BaseEntity* pEntity);
	void CacheEntityPolyInSpawnerGiveItem(int userid, const char* itemName, int spawner);
	void ModifyEntityPolyPerFrame();
	void ModifyLocalPlayerViewModel();

	void ModifyEntitySound();

	
};

namespace F { inline ModPolymorphism Polymorphism; }
