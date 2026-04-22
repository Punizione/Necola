#pragma once

#include "KeyValues.h"
#include <format>
#include <regex>
#include <cstring>
#include <string>

#include <format>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>

class CGlobal_GameUtil
{
public:
	void setConvar(const char* command);
	bool isNecolaWeapon(int weaponId);
	bool isExtraWeapon(int weaponId);
	bool isFirstWeapon(int weaponId);
	bool isSecondWeapon(int weaponId);
	int castWeaponId(const char* weaponStr);
	void setConvarGeneral(const char* keyPrefix, int value);
	bool isSpecial(const char* name);
	int getSpecialClassIdByName(const char* name);
	bool weaponIsSlot(int weaponClassId, int slot);
	//int getWeaponIDWithModelName(const char* modelName);
	int getWeaponIDWithSubtype(const char* subtype);
	int getWeaponIDWithViewModelSubtype(const char* subtype);
	//int getWeaponSlotByItemName(const char* itemName);
	int getWeaponIDWithWModel(const char* modelName);

	//int getWeaponIDByItemName(const char* itemName);
	int getWeaponIDByWeaponName(const char* itemName);
	int entityClassID2WeaponID(int EntClassID);
	const char* convertToNecolaModelName(const char* sourceModelName, int poly);
	void convertNecolaToSzName(const char* modelname, const char* szNameTmp, char* insertBuffer);
	std::string convertFilenameWithRegex(const char* pFilename);

	bool hasAddonModelWeapon(int entityClass);

	bool isTracerChangerSupport(int weaponClassId);
	bool isTracerChangerSupportWeaponID(int weaponId);
	const char* getWeaponTracerName(int weaponClassId);
	const char* getWeaponTracerTypeName(int weaponClassId);

	bool isSequenceModiferWeapon(int weaponID);
	int getDefaultActivityBySequence(int weaponID, int sequence);
	bool isNecolaActivity(int activity);


	bool isParticleCustomWeaponFireSoundSample(const char* pSample);
	int getNecolaWeaponIDWithSample(const char* pSample);

	int fastRandomPick(int a, int b);

	bool isInfected(int classId);


	const char* COUNTER_FIRST_COMMONS = "cl_glow_survivor_health_crit_colorblind_b";
	const char* COUNTER_FIRST_SPECIAL = "cl_glow_survivor_health_crit_colorblind_g";

	const char* COUNTER_SECOND_COMMONS = "cl_glow_survivor_health_crit_colorblind_r";
	const char* COUNTER_SECOND_SPECIAL = "cl_glow_survivor_health_low_colorblind_b";



	const char* STATU_HEALTH 			= "cl_glow_survivor_health_low_colorblind_r";
	const char* STATU_HEALTHCOLOR 		= "cl_glow_survivor_health_low_colorblind_g";
	const char* STATU_AMMO 				= "cl_glow_survivor_health_med_colorblind_b";



	// OLD
	// const char* COUNTER_SECOND_COMMONS = "cl_glow_survivor_health_high_g";
	// const char* COUNTER_SECOND_SPECIAL = "cl_glow_survivor_health_high_r";



	// const char* STATU_HEALTH = "cl_glow_survivor_health_med_colorblind_r";
	// const char* STATU_HEALTHCOLOR = "cl_glow_survivor_health_med_b";
	// const char* STATU_AMMO = "cl_glow_survivor_health_med_g";



	// const char* COUNTER_FIRST_BOOMER = "cl_glow_survivor_health_crit_colorblind_r";
	// const char* COUNTER_FIRST_CHARGER = "cl_glow_survivor_health_crit_b";
	// const char* COUNTER_FIRST_HUNTER = "cl_glow_survivor_health_crit_g";
	// const char* COUNTER_FIRST_JOCKEY = "cl_glow_survivor_health_crit_r";
	// const char* COUNTER_FIRST_SMOKER = "cl_glow_survivor_health_high_colorblind_b";
	// const char* COUNTER_FIRST_SPITTER = "cl_glow_survivor_health_high_colorblind_g";
	// const char* COUNTER_FIRST_TANK = "cl_glow_survivor_health_high_colorblind_r";
	// const char* COUNTER_FIRST_WITCH = "cl_glow_survivor_health_high_b";

	// const char* COUNTER_FIRST_COMMONS_HEADSHOT = "cl_glow_ghost_infected_r";
	// const char* COUNTER_FIRST_SPECIAL_HEADSHOT = "cl_glow_ghost_infected_g";


	// const char* COUNTER_SECOND_BOOMER = "cl_glow_survivor_health_low_colorblind_b";
	// const char* COUNTER_SECOND_CHARGER = "cl_glow_survivor_health_low_colorblind_g";
	// const char* COUNTER_SECOND_HUNTER = "cl_glow_survivor_health_low_colorblind_r";
	// const char* COUNTER_SECOND_JOCKEY = "cl_glow_survivor_health_low_b";
	// const char* COUNTER_SECOND_SMOKER = "cl_glow_survivor_health_low_g";
	// const char* COUNTER_SECOND_SPITTER = "cl_glow_survivor_health_low_r";
	// const char* COUNTER_SECOND_TANK = "cl_glow_survivor_health_med_colorblind_b";
	// const char* COUNTER_SECOND_WITCH = "cl_glow_survivor_health_med_colorblind_g";

	// const char* COUNTER_SECOND_COMMONS_HEADSHOT = "cl_glow_ghost_infected_b";
	// const char* COUNTER_SECOND_SPECIAL_HEADSHOT = "cl_glow_survivor_health_med_r";

};

namespace G { inline CGlobal_GameUtil Util; }


