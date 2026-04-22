#include "./utils/EntityMaxAmmoStore.h"



void EntityMaxAmmoStore::Add(int entityIndex, int maxAmmo) {
	if(store.find(entityIndex) != store.end()) {
		int currentMaxAmmo = store[entityIndex];
		if(currentMaxAmmo < maxAmmo) {
			store[entityIndex] = maxAmmo;
		}
	} else {
		store[entityIndex] = maxAmmo;
	}
}



int EntityMaxAmmoStore::Get(int entityIndex) {
	if(store.find(entityIndex) != store.end()) {
		return store[entityIndex];
	}
	return -1;
}



void EntityMaxAmmoStore::Clear() {
	store.clear();
}

void EntityMaxAmmoStore::Remove(int entityIndex ) {
	store.erase(entityIndex);
}
