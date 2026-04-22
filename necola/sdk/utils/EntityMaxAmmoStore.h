#pragma once

#include <unordered_map>

class EntityMaxAmmoStore {
private:
	// eneity index -> max ammo
	using EntityMaxAmmo = std::unordered_map<int, int>;

public:
	EntityMaxAmmo store;

public:
	EntityMaxAmmoStore() {}
	void Add(int entityIndex, int maxAmmo);
	int Get(int entityIndex);
	void Clear();

	void Remove(int entityIndex);
};

namespace G { inline EntityMaxAmmoStore MaxAmmoStore; }
