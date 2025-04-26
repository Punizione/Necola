#pragma once

#include "KeyValues.h"
#include <format>
#include "Counter.h"
class CGlobal_GameUtil
{
public:
	void setConvar(const char* command);
	bool isNecolaWeapon(int weaponId);
	bool isNecolaCounterWeapon(int weaponId);
	bool isFirstWeapon(int weaponId);
	bool isSecondWeapon(int weaponId);
	int castWeaponId(const char* weaponStr);
	void setConvarCounter(int weaponId, int entityClass);
	void setConvarGeneral(const char* keyPrefix, int value);
	void setFirst();
	void setSecond();
	void backupCounter();
	void restoreCounter();
	bool isSpecial(const char* name);
	int getSpecialClassIdByName(const char* name);

	const char* COUNTER_FIRST_COMMONS = "cl_glow_survivor_health_crit_colorblind_b";
	const char* COUNTER_FIRST_SPECIAL = "cl_glow_survivor_health_crit_colorblind_g";
	const char* COUNTER_FIRST_BOOMER = "cl_glow_survivor_health_crit_colorblind_r";
	const char* COUNTER_FIRST_CHARGER = "cl_glow_survivor_health_crit_b";
	const char* COUNTER_FIRST_HUNTER = "cl_glow_survivor_health_crit_g";
	const char* COUNTER_FIRST_JOCKEY = "cl_glow_survivor_health_crit_r";
	const char* COUNTER_FIRST_SMOKER = "cl_glow_survivor_health_high_colorblind_b";
	const char* COUNTER_FIRST_SPITTER = "cl_glow_survivor_health_high_colorblind_g";
	const char* COUNTER_FIRST_TANK = "cl_glow_survivor_health_high_colorblind_r";
	const char* COUNTER_FIRST_WITCH = "cl_glow_survivor_health_high_b";

	const char* COUNTER_FIRST_COMMONS_HEADSHOT = "cl_glow_ghost_infected_r";
	const char* COUNTER_FIRST_SPECIAL_HEADSHOT = "cl_glow_ghost_infected_g";

	const char* COUNTER_SECOND_COMMONS = "cl_glow_survivor_health_high_g";
	const char* COUNTER_SECOND_SPECIAL = "cl_glow_survivor_health_high_r";
	const char* COUNTER_SECOND_BOOMER = "cl_glow_survivor_health_low_colorblind_b";
	const char* COUNTER_SECOND_CHARGER = "cl_glow_survivor_health_low_colorblind_g";
	const char* COUNTER_SECOND_HUNTER = "cl_glow_survivor_health_low_colorblind_r";
	const char* COUNTER_SECOND_JOCKEY = "cl_glow_survivor_health_low_b";
	const char* COUNTER_SECOND_SMOKER = "cl_glow_survivor_health_low_g";
	const char* COUNTER_SECOND_SPITTER = "cl_glow_survivor_health_low_r";
	const char* COUNTER_SECOND_TANK = "cl_glow_survivor_health_med_colorblind_b";
	const char* COUNTER_SECOND_WITCH = "cl_glow_survivor_health_med_colorblind_g";

	const char* COUNTER_SECOND_COMMONS_HEADSHOT = "cl_glow_ghost_infected_b";
	const char* COUNTER_SECOND_SPECIAL_HEADSHOT = "cl_glow_survivor_health_med_r";

	const char* STATU_HEALTH = "cl_glow_survivor_health_med_colorblind_r";
	const char* STATU_HEALTHCOLOR = "cl_glow_survivor_health_med_b";
	const char* STATU_AMMO = "cl_glow_survivor_health_med_g";
};

namespace G { inline CGlobal_GameUtil Util; }