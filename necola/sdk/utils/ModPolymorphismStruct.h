#pragma once
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>

class ModPolymorphismHandle{
	public:
		int weaponId = -1;
		int entityId = -1;
		int polymorphism = 0;
	public:
		bool operator==(const ModPolymorphismStruct& other) const {
			return this->entityId == other.entityId;
		}
};


class ModPolymorphismInGameStructure {

private:
	std::unordered_map<int, ModPolymorphismHandle> entityMap;
	std::unordered_map<int, int> weaponMaxPolymorphism;

public:
	ModPolymorphismInGameStructure(){
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
	}
	void setMaxPolymorphism(int weaponId, int maxPolymorphism) {
		weaponMaxPolymorphism[weaponId] = maxPolymorphism;
	}

	bool hasPolymorphism(int weaponId) {
		return weaponMaxPolymorphism.find(weaponId) != weaponMaxPolymorphism.end();
	}

	int selectPolymorphism(int weaponId) {
		int maxPoly = getMaxPolymorphismByWeaponId(weaponId);
        
        if (maxPoly <= 0) {
            return 0;
        }
       
        return std::rand() % (maxPoly + 1);

	}


	void addEntity(int weaponId, int entityId) {
        ModPolymorphismHandle handle;
        handle.weaponId = weaponId;
        handle.entityId = entityId;
        handle.polymorphism = polymorphism;
        entityMap[entityId] = handle;
    }
	
	bool removeEntity(int entityId) {
		auto it = entityMap.find(entityId);
		if (it != entityMap.end()) {
			entityMap.erase(it);
			return true;
		}
		return false;
	}
	
	bool containsEntity(int entityId) const {
		return entityMap.find(entityId) != entityMap.end();
	}
	

	std::vector<int> getAllEntityIds() const {
		std::vector<int> ids;
		ids.reserve(entityMap.size());
		
		for (const auto& pair : entityMap) {
			ids.push_back(pair.first);
		}
		
		return ids;
	}
	
	int getPolymorphismByEntityId(int entityId) const {
		auto it = entityMap.find(entityId);
		if (it != entityMap.end()) {
			return it->second.polymorphism;
		}
		return 0;
	}
	

	size_t size() const {
		return entityMap.size();
	}
	
	void clear() {
		entityMap.clear();
	}
	
	void addEntities(const std::vector<ModPolymorphismHandle>& handles) {
		for (const auto& handle : handles) {
			addEntity(handle);
		}
	}
};

namespace G { inline ModPolymorphismStruct PolyStruct; }