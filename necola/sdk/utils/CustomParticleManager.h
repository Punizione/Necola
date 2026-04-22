#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>


#include <memory>
#include <random>
#include <chrono>



#include "../../libs/json.hpp"

using json = nlohmann::json;


class MenuOptionConfig {
public:
	std::string name;
    std::string particle;
    int type;
    int sort;
    
    MenuOptionConfig(const std::string& n, const std::string& p, int t, int s)
        : name(n), particle(p), type(t), sort(s) {}
};


class CustomParticleManager {
private:

	//	Weapon Model Name List
	//  init in instance create fill with default weapon view model
	// 	use by tracers and muzzle
	using WeaponModelList = std::unordered_set<std::string>;

	//--------------Tracers-------------------------

	//	Weapon Tracers Name List
	using WeaponTracersList = std::unordered_set<std::string>;

	// Weapon Tracers Index
	// Weapon Tracers ParticleSystem Id in Engine ParticleSystem Manager 
	using WeaponTracersPrecachedIndexList = std::unordered_set<int>;

	// Weapon Tracers in QC
	// Weapon View Model Index -> List<Tracers Name>
	using WeaponTracersExclusiveName = std::unordered_map<int, std::vector<std::string>>;

	// Weapon without Tracer in QC
	// Weapon View Model Index
	using EmptyCustomTracerModelIndexList = std::unordered_set<int>;

	// Weapon Tracers in Config
	// Weapon Necola Id -> List<Tracers Name>
	using WeaponTracersWeaponConfig = std::unordered_map<int, std::vector<std::string>>;

	//------------------------------------------------
	//---------------Muzzle---------------------------
	//	Weapon Tracers Name List
	using WeaponMuzzleList = std::unordered_set<std::string>;

	// Weapon Tracers Index
	// Weapon Tracers ParticleSystem Id in Engine ParticleSystem Manager 
	using WeaponMuzzlePrecachedIndexList = std::unordered_set<int>;

	// Weapon Tracers in QC
	// Weapon View Model Index -> List<Tracers Name>
	using WeaponMuzzleExclusiveName = std::unordered_map<int, std::vector<std::string>>;

	// Weapon Muzzle in Config
	// Weapon Necola Id -> Muzzle Name
	using WeaponMuzzleWeaponConfig = std::unordered_map<int, std::vector<std::string>>;

	// Weapon without Muzzle in QC
	// Weapon View Model Index
	using EmptyCustomMuzzleModelIndexList = std::unordered_set<int>;


	//-----------------Scene--------------------- 
	using ScenesList = std::unordered_set<std::string>;
	//------------------------------------------

	//----------------Damage Number Particle-----------
	using DamageList = std::unordered_set<std::string>;

	// Damage Particle Supprt Headshot

	//--------------------------------------------------

	//--------------Menu Options-------------------------

	using PartcileOptionsConfigList = std::vector<MenuOptionConfig>;
	
	//------------------------------------------------

public:

	// Source Weapon Model List
	WeaponModelList modelList;

	//---------------Tracers------------------
	// Source Tracers Name Set (All)
	WeaponTracersList weaponSourceTracersList;

	// Source Tracers Name in Config
	WeaponTracersWeaponConfig configSourceTracers;

	// Source Tracers Name in QC
	WeaponTracersExclusiveName qcSourceTracers;

	//---------------------------------------
	//---------------Muzzle------------------

	// Source Muzzle Name Set (All)
	WeaponMuzzleList weaponSourceMuzzleList;

	// Source Muzzle Name in Config
	WeaponMuzzleWeaponConfig configSourceMuzzle;

	// Source Muzzle Name in QC
	WeaponMuzzleExclusiveName qcSourceMuzzle;


	//----------------Scene---------------
	ScenesList sceneList;
	//------------------------------------
	
	//-----------------Damage------------------
	DamageList damageList;
	//-----------------------------------------

	//-----------------Menu Option--------------------
	// if config change in game then true
	bool hasChange;
	//--------------------------------------

private:
	//-------------------Tracers------------------------
	// Actual Tracer Name Set (Loaded by Engine)
	WeaponTracersList weaponTracersList;

	// Actual Tracer Index (Loaded by Engine)
	WeaponTracersPrecachedIndexList weaponTracersIndex;

	// Actual Tracers Name in QC
	WeaponTracersExclusiveName qcTracers;

	// Actual Tracers in Config
	WeaponTracersWeaponConfig configTracers;

	// None
	EmptyCustomTracerModelIndexList emptyCustomTracer;

	//-----------------------------------------------------
	//-------------------Muzzle------------------------
	// Actual Muzzle Name Set (Loaded By Engine)
	WeaponMuzzleList weaponMuzzleList;

	// Actual Muzzle Index (Loaded by Engine)
	WeaponMuzzlePrecachedIndexList weaponMuzzleIndex;

	// Actual Muzzle Name in QC
	WeaponMuzzleExclusiveName qcMuzzle;

	// Actual Muzzle in Config
	WeaponMuzzleWeaponConfig configMuzzle;

	// None
	EmptyCustomMuzzleModelIndexList emptyCustomMuzzle;

	//-------------Damage-----------------------
	//------------------------------------------------------

	// --------------Menu Option---------------------------
	PartcileOptionsConfigList optionConfig;
	//----------------------------------------------------

	// Local Player Muzzle Flash Timer
	float timer = 0.0;

	//--------------------------------------------------

public:
	CustomParticleManager() {
		hasChange = false;
		initDefaultWeaponTracers();
		initDefaultModels();
	}


	void addDefaultModels(const std::string& tracerName) {
		modelList.insert(tracerName);
	}

	//--------------Tracers-------------------

	void addWeaponTracers(const std::string& modelName) {
		weaponSourceTracersList.insert(modelName);
	}

	void addSourceQcTracers(int viewModelIndex, const std::string& tracerName) {
		std::istringstream iss(tracerName);
		std::string token;
		while (std::getline(iss, token, ';')) {
			if (!token.empty()) { 
				qcSourceTracers[viewModelIndex].push_back(token);
				addWeaponTracers(token);
			}
		}
	}

	

	void addConfigWeaponTracers(int necolaWeaponId, const std::string& tracerName) {

		std::istringstream iss(tracerName);
		std::string token;
		while (std::getline(iss, token, ';')) {
			if (!token.empty()) { 
				configSourceTracers[necolaWeaponId].push_back(token);
				addWeaponTracers(token);
			}
		}
	}

	void addLoadedTracerIndex(int index) {
		weaponTracersIndex.insert(index);
	}

	void addLoadedTracerName(const std::string& tracerName) {
		weaponTracersList.insert(tracerName);
	}

	bool hasTracersLoaded(const std::string& tracerName) {
		return weaponTracersList.find(tracerName) != weaponTracersList.end();
	}

	void setTracerIndex(int index) {
		weaponTracersIndex.insert(index);
	}

	bool containsTracerIndex(int index) {
		return weaponTracersIndex.find(index) != weaponTracersIndex.end();
	}

	//--------------Muzzle-------------------

	void addWeaponMuzzle(const std::string& modelName) {
		weaponSourceMuzzleList.insert(modelName);
	}

	void addSourceQcMuzzle(int viewModelIndex, const std::string& muzzleName) {
		std::istringstream iss(muzzleName);
		std::string token;
		while (std::getline(iss, token, ';')) {
			if (!token.empty()) { 
				qcSourceMuzzle[viewModelIndex].push_back(token);
				addWeaponMuzzle(token);
			}
		}
	}

	void addConfigWeaponMuzzle(int necolaWeaponId, const std::string& muzzleName) {

		std::istringstream iss(muzzleName);
		std::string token;
		while (std::getline(iss, token, ';')) {
			if (!token.empty()) { 
				configSourceMuzzle[necolaWeaponId].push_back(token);
				addWeaponMuzzle(token);
			}
		}
	}

	void addLoadedMuzzleIndex(int index) {
		weaponMuzzleIndex.insert(index);
	}

	void addLoadedMuzzleName(const std::string& muzzleName) {
		weaponMuzzleList.insert(muzzleName);
	}

	bool hasMuzzleLoaded(const std::string& muzzleName) {
		return weaponMuzzleList.find(muzzleName) != weaponMuzzleList.end();
	}

	void setMuzzleIndex(int index) {
		weaponMuzzleIndex.insert(index);
	}

	bool containsMuzzleIndex(int index) {
		return weaponMuzzleIndex.find(index) != weaponMuzzleIndex.end();
	}


	int rand_int(int min, int max) {
	    static std::minstd_rand engine(std::chrono::system_clock::now().time_since_epoch().count());
	    std::uniform_int_distribution<int> dist(min, max);
	    return dist(engine);
	}

		// void cache(int index, const std::string& tracer);
	void addEmptyCustomQcTracerModelIndex(int index) {
		emptyCustomTracer.insert(index);
	}
	void addEmptyCustomQcMuzzleModelIndex(int index) {
		emptyCustomMuzzle.insert(index);
	}


	bool isEmptyCustomQcTracer(int index) {
		return emptyCustomTracer.find(index) != emptyCustomTracer.end();
	}
	bool isEmptyCustomQcMuzzle(int index) {
		return emptyCustomMuzzle.find(index) != emptyCustomMuzzle.end();
	}

	void optionResetMuzzle(int weaponId, const std::string& muzzleName);
	void optionResetTracer(int weaponId, const std::string& tracerName);
	

	//----------------------Scene--------------------
	void addScene(const std::string& modelName) {
		sceneList.insert(modelName);
	}
	//-----------------------------------------------

	//---------------------Damage--------------------------
	void addDamage(const std::string& modelName) {
		damageList.insert(modelName);
	}

	
	//-----------------------------------------------------


	bool initializeFromJson(const json& jsonData);
	bool initializeFromJsonFile(const std::string& filename);

	bool exportToFile(const std::string& filename);
	


	void clear();
	void clearSource();

	bool hasTracersQc(int index);
	const std::string& getTracersQc(int index);

	bool hasTracersConfig(int weaponID);
	const std::string& getTracersConfig(int weaponID);


	bool hasMuzzleQc(int index);
	const std::string& getMuzzleQc(int index);

	bool hasMuzzleConfig(int weaponID);
	const std::string& getMuzzleConfig(int weaponID);


	void setTimer(float pTimer) {
		timer = pTimer;
	}

	float getTimer() {
		return timer;
	}

	void Settle();

private:
	bool menuOptionNameExists(const std::string& name) const {
        for (const auto& obj : optionConfig) {
            if (obj.name == name) {
                return true;
            }
        }
        return false;
    }
    json toJsonData();
    std::string vectorToString(const std::vector<std::string>& vec, const std::string& delimiter);
    const char* getNCLName(int necolaId);

public:
	bool menuOptionInsert(const std::string& name, const std::string& particle, int type, int sort) {
        if (menuOptionNameExists(name)) {
            return false;
        }
        if(type == 1) {
        	addWeaponTracers(particle);
        } else if(type == 2) {
        	addWeaponMuzzle(particle);
        } else if(type == 3) {
        	addScene(particle);
        } else if(type == 4) {
        	addDamage(particle);
        }
        // 直接添加对象
        optionConfig.emplace_back(name, particle, type, sort);
        return true;
    }



    std::vector<MenuOptionConfig> getMenuOptionByTypeSorted(int filterType) const {
    	std::vector<MenuOptionConfig> result;
    	for (const auto& obj : optionConfig) {
            if (obj.type == filterType) {
                result.push_back(obj);
            }
        }
        std::sort(result.begin(), result.end(),
            [](const MenuOptionConfig& a, const MenuOptionConfig& b) {
                return a.sort > b.sort;
            });
      	return result;
    }

    void menuOptionClear() {
        optionConfig.clear();
    }

    size_t menuOptionSize() const {
        return optionConfig.size();
    }

    bool menuOptionEmpty() const {
        return optionConfig.empty();
    }

	void initDefaultWeaponTracers() {

		// weapon class tracer
		addWeaponTracers("weapon_tracers_pistol");
		addWeaponTracers("weapon_tracers_pistol_dual");
		addWeaponTracers("weapon_tracers_magnum");

		addWeaponTracers("weapon_tracers_uzi");
		addWeaponTracers("weapon_tracers_mac10");
		addWeaponTracers("weapon_tracers_mp5");

		addWeaponTracers("weapon_tracers_pump");
		addWeaponTracers("weapon_tracers_chrome");
		addWeaponTracers("weapon_tracers_auto");
		addWeaponTracers("weapon_tracers_spas");

		addWeaponTracers("weapon_tracers_m16");
		addWeaponTracers("weapon_tracers_ak47");
		addWeaponTracers("weapon_tracers_scar");
		addWeaponTracers("weapon_tracers_sg552");

		addWeaponTracers("weapon_tracers_mini14");
		addWeaponTracers("weapon_tracers_military");
		addWeaponTracers("weapon_tracers_scout");
		addWeaponTracers("weapon_tracers_awp");

		addWeaponTracers("weapon_tracers_m60");

		// weapon type tracer
		addWeaponTracers("weapon_tracers_pistol_s");
		addWeaponTracers("weapon_tracers_smg");
		addWeaponTracers("weapon_tracers_rifle");
		addWeaponTracers("weapon_tracers_shotgun");
		addWeaponTracers("weapon_tracers_sniper");

	}

	void initDefaultModels() {
		addDefaultModels( "models/v_models/v_pistola.mdl");
		addDefaultModels( "models/v_models/v_dual_pistola.mdl");
		addDefaultModels( "models/v_models/v_smg.mdl");
		addDefaultModels( "models/v_models/v_pumpshotgun.mdl");
		addDefaultModels( "models/v_models/v_autoshotgun.mdl");
		addDefaultModels( "models/v_models/v_rifle.mdl");
		addDefaultModels( "models/v_models/v_huntingrifle.mdl");
		addDefaultModels( "models/v_models/v_silenced_smg.mdl");
		addDefaultModels( "models/v_models/v_shotgun_chrome.mdl");
		addDefaultModels( "models/v_models/v_desert_rifle.mdl");
		addDefaultModels( "models/v_models/v_sniper_military.mdl");
		addDefaultModels( "models/v_models/v_shotgun_spas.mdl");
		addDefaultModels( "models/v_models/v_rifle_ak47.mdl");
		addDefaultModels( "models/v_models/v_desert_eagle.mdl");
		addDefaultModels( "models/v_models/v_smg_mp5.mdl");
		addDefaultModels( "models/v_models/v_rif_sg552.mdl");
		addDefaultModels( "models/v_models/v_snip_awp.mdl");
		addDefaultModels( "models/v_models/v_snip_scout.mdl");
		addDefaultModels( "models/v_models/v_m60.mdl");
	}
};

namespace G { inline CustomParticleManager Particle; }
