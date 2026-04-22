#include "./utils/WeaponModelsManager.h"
#include <fstream>
#include <sstream>
#include <iostream>


bool WeaponModelsManager::initializeFromJson(const json& jsonData) {
	weaponPolymorphisCount.clear();
	if(jsonData.is_array()) {
		if(!jsonData.empty()){
			for(const auto& element : jsonData) {
				int weaponId = element.value("id", 0);
				int polymorphism = element.value("polymorphism", 0);
				if( polymorphism != 0 ) {
					weaponPolymorphisCount[weaponId] = polymorphism;
				}
			}
			return true;
		}
		
	}
	return false;
}

bool WeaponModelsManager::initializeFromJsonFile(const std::string& filename) {
	try {
		std::ifstream file(filename);
		if (!file.is_open()) {
			return false;
		}
		json jsonData;
		file >> jsonData;
		file.close();
		return initializeFromJson(jsonData);
	} catch (const std::exception& e) {
		return false;
	}
}


json WeaponModelsManager::toJson() const {
	json result;

	return result;
}

void WeaponModelsManager::clear() {

}

bool WeaponModelsManager::saveToJsonFile(const std::string& filename, bool prettyPrint) const {
	try {
		std::ofstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Error opening file for writing: " << filename << std::endl;
			return false;
		}
		
		if (prettyPrint) {
			file << toJson().dump(4);
		} else {
			file << toJson().dump();
		}
		
		file.close();
		return true;
	} catch (const std::exception& e) {
		return false;
	}
}

bool WeaponModelsManager::loadConfig(){
	return false;
}



bool WeaponModelsManager::hasPolymorphis(int weaponId) {
	return weaponPolymorphisCount.find(weaponId) != weaponPolymorphisCount.end();
}

bool WeaponModelsManager::containsEntity(int entityId) {
	return entityPolymorphis.find(entityId) != entityPolymorphis.end();
}

int WeaponModelsManager::getPolyByEntityID(int entityId) {
	if(containsEntity(entityId)) {
		return entityPolymorphis[entityId];
	}
	return 0;
}

int WeaponModelsManager::selectPolymorphism(int weaponId) {
	if(hasPolymorphis(weaponId)) {
		int maxCount = weaponPolymorphisCount[weaponId];
		if(maxCount > 0) {
			return rand_int(0, maxCount);
		}
	}
	return 0;
}

void WeaponModelsManager::addEntity(int entityId, int poly) {
	entityPolymorphis[entityId] = poly;
}

void WeaponModelsManager::removeEntity(int entityId) {
	entityPolymorphis.erase(entityId);
}

void WeaponModelsManager::clearEntity() {
	entityPolymorphis.clear();
}

// void WeaponModelsManager::backupLocalWeapon(int weaponSlot, int poly) {
// 	backupEntityPolymorphis[entityId] = poly;
// }

// void WeaponModelsManager::restoreLocalWeapon() {
// 	clearEntity();
// 	for(auto it = backupEntityPolymorphis.begin(); it != backupEntityPolymorphis.end(); ++it) {
// 		if(it->second != -1) {
// 			entityPolymorphis[it->first] = it->second;
// 		}
		
// 	}
// }


int WeaponModelsManager::getPolyIndexBySourceIndex(int modelIndex, int poly) {
	if(sourceModelIndexPolymorphis.find(modelIndex) != sourceModelIndexPolymorphis.end()) {
		PolymorphisModelIndex polymorphisModelIndex = sourceModelIndexPolymorphis[modelIndex];
		if(polymorphisModelIndex.find(poly) != polymorphisModelIndex.end()) {
			return polymorphisModelIndex[poly];
		}
	}
	return modelIndex;
}

void WeaponModelsManager::addModelIndex(int sourceModelIndex, int poly, int necolaModelIndex) {
	sourceModelIndexPolymorphis[sourceModelIndex][poly] = necolaModelIndex;
}

void WeaponModelsManager::clearModelIndex() {
	sourceModelIndexPolymorphis.clear();
}


bool WeaponModelsManager::hasModelName(int weaponId) {
	return weaponModelMap.find(weaponId) != weaponModelMap.end();
}

const std::vector<std::string>& WeaponModelsManager::getModelNames(int weaponId) {
	return weaponModelMap.at(weaponId);
}

bool WeaponModelsManager::hasPrecache(int playerEntityId, int weaponId) {
	if(userPolymorphisPrecache.find(playerEntityId) != userPolymorphisPrecache.end()) {
		WeaponPolymorphisPrecache weaponPolymorphisPrecache = userPolymorphisPrecache[playerEntityId];
		if(weaponPolymorphisPrecache.find(weaponId) != weaponPolymorphisPrecache.end()) {
			return true;
		}
	}
	return false;
}


void WeaponModelsManager::addPrecache(int playerEntityId, int weaponId, int poly ) {
	userPolymorphisPrecache[playerEntityId][weaponId] = poly;
}

int WeaponModelsManager::popPrecache(int playerEntityId, int weaponId) {
	int poly = 0;
	
	WeaponPolymorphisPrecache& weaponPolymorphisPrecache = userPolymorphisPrecache[playerEntityId];
	poly = weaponPolymorphisPrecache[weaponId];
	weaponPolymorphisPrecache.erase(weaponId);
	
	return poly;
}

void WeaponModelsManager::clearPrecache() {
	userPolymorphisPrecache.clear();
}
