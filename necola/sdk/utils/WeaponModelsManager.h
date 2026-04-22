#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <random>
#include <chrono>
#include "../../libs/json.hpp"

using json = nlohmann::json;


class WeaponModelsManager {
private:

	// WEAPON_ID -> Polymorphis Max Count
	using WeaponPolymorphisCount = std::unordered_map<int, int>;

	// ENTITY_ID -> Polymorphis Number
	using EntityPolymorphis = std::unordered_map<int, int>;

	//Source Model Index -> (Polymorphis Number,  Necola Model Index)
	using PolymorphisModelIndex = std::unordered_map<int, int>;
	using SourceModelIndexPolymorphis = std::unordered_map<int, PolymorphisModelIndex>;

	//Source Model Name -> (Polymorphis Number,  Necola Model Name)
	//using PolymorphisModelNameIndex = std::unordered_map<int, int>;


	//User Polymorphis PreCache -> (WEAPON_ID, Polymorphis Number)
	using WeaponPolymorphisPrecache = std::unordered_map<int, int>;
	using UserPolymorphisPrecache = std::unordered_map<int, WeaponPolymorphisPrecache>;

	//WEAPON_ID -> List (SourceModelName)
	using WeaponModelMap = std::unordered_map<int, std::vector<std::string>>;

	//WEAPON_ID -> (Polymorphis Number, Necola Sound Index)
	using PolymorphisSoundIndex = std::unordered_map<int, int>;
	using SourceSoundIndexPolymorphis = std::unordered_map<int, PolymorphisSoundIndex>;


	//

public:
	WeaponPolymorphisCount weaponPolymorphisCount;
	EntityPolymorphis entityPolymorphis;

private:

	SourceModelIndexPolymorphis sourceModelIndexPolymorphis;
	UserPolymorphisPrecache userPolymorphisPrecache;
	WeaponModelMap weaponModelMap;
	
public:
	WeaponModelsManager() {
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		initSourceModelName();
	}

	void addSourceModel(int weaponId, const std::string& modelName) {
		weaponModelMap[weaponId].push_back(modelName);
	}
	

	bool initializeFromJson(const json& jsonData);
	bool initializeFromJsonFile(const std::string& filename);
	bool loadConfig();

	//weaponID -> poly
	bool hasPolymorphis(int weaponId);
	int selectPolymorphism(int weaponId);

	// entity -> poly
	void addEntity(int entityId, int poly);
	bool containsEntity(int entityId);
	int getPolyByEntityID(int entityId);
	void removeEntity(int entityId);
	void clearEntity();

	//void backupLocalWeapon(int entityId, int poly);
	//void restoreLocalWeapon();

	// model index -> poly
	int getPolyIndexBySourceIndex(int modelIndex, int poly);
	void addModelIndex(int sourceModelIndex, int poly, int necolaModelIndex);
	void clearModelIndex();

	/*void addModelNameIndex(std::string modelName, int index);*/

	// model name
	bool hasModelName(int weaponId);
	const std::vector<std::string>& getModelNames(int weaponId);


	// precache 
	bool hasPrecache(int playerEntityId, int weaponId);
	void addPrecache(int playerEntityId, int weaponId, int poly);
	int popPrecache(int playerEntityId, int weaponId);
	void clearPrecache();
	
	void clear();
	
	json toJson() const;
	
	bool saveToJsonFile(const std::string& filename, bool prettyPrint = false) const;

	int rand_int(int min, int max) {
	    static std::minstd_rand engine(std::chrono::system_clock::now().time_since_epoch().count());
	    std::uniform_int_distribution<int> dist(min, max);
	    return dist(engine);
	}
	
	// èŽ·å–å†…éƒ¨æ•°æ®ç»“æž„çš„å¼•ç”?
	//const WeaponPolyMap& getData() const { return weaponModels; }

	void initSourceModelName() {
		//pistol
		addSourceModel(1, "models/v_models/v_pistola.mdl");
		addSourceModel(1, "models/v_models/v_dual_pistola.mdl");
		addSourceModel(1, "models/w_models/weapons/w_pistol_b.mdl");
		addSourceModel(1, "models/w_models/weapons/w_pistol_a_dual.mdl");
		addSourceModel(1, "models/w_models/weapons/w_pistol_a.mdl");

		//uzi
		addSourceModel(2, "models/v_models/v_smg.mdl");
		addSourceModel(2, "models/w_models/weapons/w_smg_uzi.mdl");

		//pumpshotgun
		addSourceModel(3, "models/v_models/v_pumpshotgun.mdl");
		addSourceModel(3, "models/w_models/weapons/w_shotgun.mdl");

		//autoshotgun
		addSourceModel(4, "models/v_models/v_autoshotgun.mdl");
		addSourceModel(4, "models/w_models/weapons/w_autoshot_m4super.mdl");

		//m16
		addSourceModel(5, "models/v_models/v_rifle.mdl");
		addSourceModel(5, "models/w_models/weapons/w_rifle_m16a2.mdl");

		//hunting rifle
		addSourceModel(6, "models/v_models/v_huntingrifle.mdl");
		addSourceModel(6, "models/w_models/weapons/w_sniper_mini14.mdl");

		//mac10
		addSourceModel(7, "models/v_models/v_silenced_smg.mdl");
		addSourceModel(7, "models/w_models/weapons/w_smg_a.mdl");

		//chrome shotgun
		addSourceModel(8, "models/v_models/v_shotgun_chrome.mdl");
		addSourceModel(8, "models/w_models/weapons/w_pumpshotgun_a.mdl");

		//scar
		addSourceModel(9, "models/v_models/v_desert_rifle.mdl");
		addSourceModel(9, "models/w_models/weapons/w_desert_rifle.mdl");

		//military sniper
		addSourceModel(10, "models/v_models/v_sniper_military.mdl");
		addSourceModel(10, "models/w_models/weapons/w_sniper_military.mdl");

		//spas
		addSourceModel(11, "models/v_models/v_shotgun_spas.mdl");
		addSourceModel(11, "models/w_models/weapons/w_shotgun_spas.mdl");

		//first aid
		addSourceModel(12, "models/v_models/v_medkit.mdl");
		addSourceModel(12, "models/w_models/weapons/w_eq_medkit.mdl");

		//molotov
		addSourceModel(13, "models/v_models/v_molotov.mdl");
		addSourceModel(13, "models/w_models/weapons/w_eq_molotov.mdl");

		//pipebome
		addSourceModel(14, "models/v_models/v_pipebomb.mdl");
		addSourceModel(14, "models/w_models/weapons/w_eq_pipebomb.mdl");

		//pills
		addSourceModel(15, "models/v_models/v_painpills.mdl");
		addSourceModel(15, "models/w_models/weapons/w_eq_painpills.mdl");

		//gascan
		addSourceModel(16, "models/props_junk/dieselcan.mdl");
		addSourceModel(16, "models/props_junk/gascan001a.mdl");
		addSourceModel(16, "models/props_unique/wooden_barricade_gascans.mdl");

		//propane tank
		addSourceModel(17, "models/props_junk/propanecanister001a.mdl");

		//oxygen tank
		addSourceModel(18, "models/props_equipment/oxygentank01.mdl");

		//chainsaw
		addSourceModel(20, "models/weapons/melee/v_chainsaw.mdl");
		addSourceModel(20, "models/weapons/melee/w_chainsaw.mdl");

		//grenade launcher
		addSourceModel(21, "models/v_models/v_grenade_launcher.mdl");
		addSourceModel(21, "models/w_models/weapons/w_grenade_launcher.mdl");

		//adrenaline
		addSourceModel(23, "models/v_models/v_adrenaline.mdl");
		addSourceModel(23, "models/w_models/weapons/w_eq_adrenaline.mdl");

		//defibrillator
		addSourceModel(24, "models/v_models/v_defibrillator.mdl");
		addSourceModel(24, "models/w_models/weapons/w_eq_defibrillator.mdl");
		addSourceModel(24, "models/w_models/weapons/w_eq_defibrillator_no_paddles.mdl");

		//vomitjar
		addSourceModel(25, "models/v_models/v_bile_flask.mdl");
		addSourceModel(25, "models/w_models/weapons/w_eq_bile_flask.mdl");

		//ak47
		addSourceModel(26, "models/v_models/v_rifle_ak47.mdl");
		addSourceModel(26, "models/w_models/weapons/w_rifle_ak47.mdl");

		//cola
		addSourceModel(27, "models/v_models/v_cola.mdl");
		addSourceModel(27, "models/w_models/weapons/w_cola.mdl");

		//gnome
		addSourceModel(28, "models/weapons/melee/v_gnome.mdl");
		addSourceModel(28, "models/props_junk/gnome.mdl");

		//firework
		addSourceModel(29, "models/props_junk/explosive_box001.mdl");

		//upgradpack incendiary
		addSourceModel(30, "models/v_models/v_incendiary_ammopack.mdl");
		addSourceModel(30, "models/w_models/weapons/w_eq_incendiary_ammopack.mdl");
		addSourceModel(30, "models/props/terror/incendiary_ammo.mdl");

		//upgradpack explosive
		addSourceModel(31, "models/v_models/v_explosive_ammopack.mdl");
		addSourceModel(31, "models/w_models/weapons/w_eq_explosive_ammopack.mdl");
		addSourceModel(31, "models/props/terror/exploding_ammo.mdl");

		//magnum
		addSourceModel(32, "models/v_models/v_desert_eagle.mdl");
		addSourceModel(32, "models/w_models/weapons/w_desert_eagle.mdl");

		//mp5
		addSourceModel(33, "models/v_models/v_smg_mp5.mdl");
		addSourceModel(33, "models/w_models/weapons/w_smg_mp5.mdl");

		//sg552
		addSourceModel(34, "models/v_models/v_rif_sg552.mdl");
		addSourceModel(34, "models/w_models/weapons/w_rifle_sg552.mdl");

		//awp
		addSourceModel(35, "models/v_models/v_snip_awp.mdl");
		addSourceModel(35, "models/w_models/weapons/w_sniper_awp.mdl");

		//scout
		addSourceModel(36, "models/v_models/v_snip_scout.mdl");
		addSourceModel(36, "models/w_models/weapons/w_sniper_scout.mdl");

		//m60
		addSourceModel(37, "models/v_models/v_m60.mdl");
		addSourceModel(37, "models/w_models/weapons/w_m60.mdl");

		//bat
		addSourceModel(60, "models/weapons/melee/v_bat.mdl");
		addSourceModel(60, "models/weapons/melee/w_bat.mdl");

		//cricket bat
		addSourceModel(61, "models/weapons/melee/v_cricket_bat.mdl");
		addSourceModel(61, "models/weapons/melee/w_cricket_bat.mdl");

		//crowbar
		addSourceModel(62, "models/weapons/melee/v_crowbar.mdl");
		addSourceModel(62, "models/weapons/melee/w_crowbar.mdl");

		//guitar
		addSourceModel(63, "models/weapons/melee/v_electric_guitar.mdl");
		addSourceModel(63, "models/weapons/melee/w_electric_guitar.mdl");

		//fireaxe
		addSourceModel(64, "models/weapons/melee/v_fireaxe.mdl");
		addSourceModel(64, "models/weapons/melee/w_fireaxe.mdl");

		//pan
		addSourceModel(65, "models/weapons/melee/v_frying_pan.mdl");
		addSourceModel(65, "models/weapons/melee/w_frying_pan.mdl");

		//golfglub
		addSourceModel(66, "models/weapons/melee/v_golfclub.mdl");
		addSourceModel(66, "models/weapons/melee/w_golfclub.mdl");

		//katana
		addSourceModel(67, "models/weapons/melee/v_katana.mdl");
		addSourceModel(67, "models/weapons/melee/w_katana.mdl");

		//knife
		addSourceModel(68, "models/v_models/v_knife_t.mdl");
		addSourceModel(68, "models/w_models/weapons/w_knife_t.mdl");

		//machete
		addSourceModel(69, "models/weapons/melee/v_machete.mdl");
		addSourceModel(69, "models/weapons/melee/w_machete.mdl");

		//pitchfork
		addSourceModel(70, "models/weapons/melee/v_pitchfork.mdl");
		addSourceModel(70, "models/weapons/melee/w_pitchfork.mdl");

		//shield
		addSourceModel(71, "models/weapons/melee/v_riotshield.mdl");
		addSourceModel(71, "models/weapons/melee/w_riotshield.mdl");

		//shovel
		addSourceModel(72, "models/weapons/melee/v_shovel.mdl");
		addSourceModel(72, "models/weapons/melee/w_shovel.mdl");

		//tonfa
		addSourceModel(73, "models/weapons/melee/v_tonfa.mdl");
		addSourceModel(73, "models/weapons/melee/w_tonfa.mdl");

		//ammo_spawn
		addSourceModel(80, "models/props/terror/ammo_stack.mdl");
		addSourceModel(80, "models/props_unique/spawn_apartment/coffeeammo.mdl");
		addSourceModel(80, "models/props/de_prodigy/ammo_can_02.mdl");

		//laser sights
		addSourceModel(81, "models/w_models/weapons/w_laser_sights.mdl");


	}

};

namespace G { inline WeaponModelsManager WeaponPoly; }
