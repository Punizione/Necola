#include "./utils/GameUtil.h"
#include <format>
#include <iostream>

void CGlobal_GameUtil::setConvar(const char* command)
{
	// std::cout << command << std::endl;
	const HWND m_hEngine = FindWindowA("Valve001", 0);
	COPYDATASTRUCT m_cData;
	m_cData.cbData = strlen(command) + 1;
	m_cData.dwData = 0;
	m_cData.lpData = (void*)command;

	SendMessageA(m_hEngine, WM_COPYDATA, 0, (LPARAM)&m_cData);

}


void CGlobal_GameUtil::setConvarGeneral(const char* key, int value) {
	setConvar(std::format("{0} {1}", key, value).c_str());
}

bool CGlobal_GameUtil::isNecolaWeapon(int weaponId) {
	return weaponId == WEAPON_PISTOL || weaponId == WEAPON_DEAGLE 
		|| weaponId == WEAPON_UZI || weaponId == WEAPON_MAC10 || weaponId == WEAPON_MP5 
		|| weaponId == WEAPON_PUMP_SHOTGUN || weaponId == WEAPON_CHROME_SHOTGUN
		|| weaponId == WEAPON_AUTO_SHOTGUN || weaponId == WEAPON_SPAS 
		|| weaponId == WEAPON_SCAR || weaponId == WEAPON_AK47 || weaponId == WEAPON_M16A1 || weaponId == WEAPON_SSG552
		|| weaponId == WEAPON_HUNTING_RIFLE  || weaponId == WEAPON_MILITARY_SNIPER
		|| weaponId == WEAPON_AWP  || weaponId == WEAPON_SCOUT
		|| weaponId == WEAPON_M60 || weaponId == WEAPON_GRENADE_LAUNCHER;
}
bool CGlobal_GameUtil::isNecolaCounterWeapon(int weaponId) {
	return isNecolaWeapon(weaponId) || weaponId == WEAPON_MELEE || weaponId == WEAPON_CHAINSAW;
}

bool CGlobal_GameUtil::isFirstWeapon(int weaponId) {
	return  weaponId == WEAPON_UZI || weaponId == WEAPON_MAC10 || weaponId == WEAPON_MP5 
		|| weaponId == WEAPON_PUMP_SHOTGUN || weaponId == WEAPON_CHROME_SHOTGUN
		|| weaponId == WEAPON_AUTO_SHOTGUN || weaponId == WEAPON_SPAS 
		|| weaponId == WEAPON_SCAR || weaponId == WEAPON_AK47 || weaponId == WEAPON_M16A1 || weaponId == WEAPON_SSG552
		|| weaponId == WEAPON_HUNTING_RIFLE  || weaponId == WEAPON_MILITARY_SNIPER
		|| weaponId == WEAPON_AWP  || weaponId == WEAPON_SCOUT
		|| weaponId == WEAPON_M60 || weaponId == WEAPON_GRENADE_LAUNCHER;
}

bool CGlobal_GameUtil::isSecondWeapon(int weaponId) {
	return   weaponId == WEAPON_PISTOL || weaponId == WEAPON_DEAGLE || weaponId == WEAPON_MELEE || weaponId == WEAPON_CHAINSAW;
}

int CGlobal_GameUtil::castWeaponId(const char* weaponStr) {
	int weaponId = -1;
	if(strcmp("pistol", weaponStr) == 0|| strcmp("dual_pistols", weaponStr)  == 0) {
		weaponId = WEAPON_PISTOL;
	} else if(strcmp("pistol_magnum", weaponStr)  == 0) {
		weaponId = WEAPON_DEAGLE;
	} else if(strcmp("smg", weaponStr)  == 0) {
		weaponId = WEAPON_UZI;
	} else if(strcmp("smg_silenced", weaponStr) == 0) {
		weaponId = WEAPON_MAC10;
	} else if(strcmp("smg_mp5", weaponStr) == 0) {
		weaponId = WEAPON_MP5;
	} else if(strcmp("pumpshotgun", weaponStr) == 0) {
		weaponId = WEAPON_PUMP_SHOTGUN;
	} else if(strcmp("shotgun_chrome", weaponStr) == 0) {
		weaponId = WEAPON_CHROME_SHOTGUN;
	} else if(strcmp("autoshotgun", weaponStr) == 0) {
		weaponId = WEAPON_AUTO_SHOTGUN;
	} else if(strcmp("shotegun_spas", weaponStr) == 0) {
		weaponId = WEAPON_SPAS;
	} else if(strcmp("rifle", weaponStr) == 0) {
		weaponId = WEAPON_M16A1;
	} else if(strcmp("rifle_desert", weaponStr) == 0) {
		weaponId = WEAPON_SCAR;
	} else if(strcmp("rifle_ak47", weaponStr) == 0) {
		weaponId = WEAPON_AK47;
	} else if(strcmp("rifle_sg552", weaponStr) == 0) {
		weaponId = WEAPON_SSG552;
	} else if(strcmp("hunting_rifle", weaponStr) == 0) {
		weaponId = WEAPON_HUNTING_RIFLE;
	} else if(strcmp("sniper_military", weaponStr) == 0) {
		weaponId = WEAPON_MILITARY_SNIPER;
	} else if(strcmp("sniper_scout", weaponStr) == 0) {
		weaponId = WEAPON_SCOUT;
	} else if(strcmp("sniper_awp", weaponStr) == 0) {
		weaponId = WEAPON_AWP;
	} else if(strcmp("rifle_m60", weaponStr) == 0) {
		weaponId = WEAPON_M60;
	} else if(strcmp("grenade_launcher", weaponStr) == 0 || strcmp("grenade_launcher_projectile", weaponStr) == 0) {
		weaponId = WEAPON_GRENADE_LAUNCHER;
	} else if(strcmp("melee", weaponStr) == 0) {
		weaponId = WEAPON_MELEE;
	} else if(strcmp("chainsaw", weaponStr) == 0) {
		weaponId = WEAPON_CHAINSAW;
	}
	
	return weaponId;
}

void CGlobal_GameUtil::setFirst() {
	setConvarGeneral(COUNTER_FIRST_COMMONS, Necola_Counter::first_commons);
	setConvarGeneral(COUNTER_FIRST_SPECIAL, Necola_Counter::firstSpecial());
	setConvarGeneral(COUNTER_FIRST_BOOMER, Necola_Counter::first_boomer);
	setConvarGeneral(COUNTER_FIRST_CHARGER, Necola_Counter::first_charger);
	setConvarGeneral(COUNTER_FIRST_HUNTER, Necola_Counter::first_hunter);
	setConvarGeneral(COUNTER_FIRST_JOCKEY, Necola_Counter::first_jockey);
	setConvarGeneral(COUNTER_FIRST_SMOKER, Necola_Counter::first_smoker);
	setConvarGeneral(COUNTER_FIRST_SPITTER, Necola_Counter::first_spitter);
	setConvarGeneral(COUNTER_FIRST_TANK, Necola_Counter::first_tank);
	setConvarGeneral(COUNTER_FIRST_WITCH, Necola_Counter::first_witch);

	setConvarGeneral(COUNTER_FIRST_COMMONS_HEADSHOT, Necola_Counter::first_commons_headshot);
	setConvarGeneral(COUNTER_FIRST_SPECIAL_HEADSHOT, Necola_Counter::first_special_headshot);
}

void CGlobal_GameUtil::setSecond() {
	setConvarGeneral(COUNTER_SECOND_COMMONS, Necola_Counter::second_commons);
	setConvarGeneral(COUNTER_SECOND_SPECIAL, Necola_Counter::secondSpecial());
	setConvarGeneral(COUNTER_SECOND_BOOMER, Necola_Counter::second_boomer);
	setConvarGeneral(COUNTER_SECOND_CHARGER, Necola_Counter::second_charger);
	setConvarGeneral(COUNTER_SECOND_HUNTER, Necola_Counter::second_hunter);
	setConvarGeneral(COUNTER_SECOND_JOCKEY, Necola_Counter::second_jockey);
	setConvarGeneral(COUNTER_SECOND_SMOKER, Necola_Counter::second_smoker);
	setConvarGeneral(COUNTER_SECOND_SPITTER, Necola_Counter::second_spitter);
	setConvarGeneral(COUNTER_SECOND_TANK, Necola_Counter::second_tank);
	setConvarGeneral(COUNTER_SECOND_WITCH, Necola_Counter::second_witch);

	setConvarGeneral(COUNTER_SECOND_COMMONS_HEADSHOT, Necola_Counter::second_commons_headshot);
	setConvarGeneral(COUNTER_SECOND_SPECIAL_HEADSHOT, Necola_Counter::second_special_headshot);
}

void CGlobal_GameUtil::setConvarCounter(int weaponId, int entityClass) {
	if(isSecondWeapon(weaponId)) {
		if(entityClass == Infected) {
			setConvarGeneral(COUNTER_SECOND_COMMONS, Necola_Counter::second_commons);
			setConvarGeneral(COUNTER_SECOND_COMMONS_HEADSHOT, Necola_Counter::second_commons_headshot);
		} else {
			setConvarGeneral(COUNTER_SECOND_SPECIAL, Necola_Counter::secondSpecial());
			setConvarGeneral(COUNTER_SECOND_SPECIAL_HEADSHOT, Necola_Counter::second_special_headshot);
			switch (entityClass)
			{
				case Boomer:
					setConvarGeneral(COUNTER_SECOND_BOOMER, Necola_Counter::second_boomer);
					break;
				case Charger:
					setConvarGeneral(COUNTER_SECOND_CHARGER, Necola_Counter::second_charger);
					break;
				case Hunter:
					setConvarGeneral(COUNTER_SECOND_HUNTER, Necola_Counter::second_hunter);
					break;
				case Jockey:
					setConvarGeneral(COUNTER_SECOND_JOCKEY, Necola_Counter::second_jockey);
					break;
				case Smoker:
					setConvarGeneral(COUNTER_SECOND_SMOKER, Necola_Counter::second_smoker);
					break;
				case Spitter:
					setConvarGeneral(COUNTER_SECOND_SPITTER, Necola_Counter::second_spitter);
					break;
				case Tank:
					setConvarGeneral(COUNTER_SECOND_TANK, Necola_Counter::second_tank);
					break;
				case Witch:
					setConvarGeneral(COUNTER_SECOND_WITCH, Necola_Counter::second_witch);
					setConvarGeneral(COUNTER_SECOND_COMMONS, Necola_Counter::second_commons);
					break;
			}
		}
	} else {
		if(entityClass == Infected) {
			setConvarGeneral(COUNTER_FIRST_COMMONS, Necola_Counter::first_commons);
			setConvarGeneral(COUNTER_FIRST_COMMONS_HEADSHOT, Necola_Counter::first_commons_headshot);
		} else {
			setConvarGeneral(COUNTER_FIRST_SPECIAL, Necola_Counter::firstSpecial());
			setConvarGeneral(COUNTER_FIRST_SPECIAL_HEADSHOT, Necola_Counter::first_special_headshot);
			switch (entityClass)
			{
				case Boomer:
					setConvarGeneral(COUNTER_FIRST_BOOMER, Necola_Counter::first_boomer);
					break;
				case Charger:
					setConvarGeneral(COUNTER_FIRST_CHARGER, Necola_Counter::first_charger);
					break;
				case Hunter:
					setConvarGeneral(COUNTER_FIRST_HUNTER, Necola_Counter::first_hunter);
					break;
				case Jockey:
					setConvarGeneral(COUNTER_FIRST_JOCKEY, Necola_Counter::first_jockey);
					break;
				case Smoker:
					setConvarGeneral(COUNTER_FIRST_SMOKER, Necola_Counter::first_smoker);
					break;
				case Spitter:
					setConvarGeneral(COUNTER_FIRST_SPITTER, Necola_Counter::first_spitter);
					break;
				case Tank:
					setConvarGeneral(COUNTER_FIRST_TANK, Necola_Counter::first_tank);
					break;
				case Witch:
					setConvarGeneral(COUNTER_FIRST_WITCH, Necola_Counter::first_witch);
					setConvarGeneral(COUNTER_FIRST_COMMONS, Necola_Counter::first_commons);
					break;
			}
		}
	}

	
}

bool CGlobal_GameUtil::isSpecial(const char* name) {
	return strcmp("Boomer", name) == 0 || strcmp("Charger", name) == 0 || strcmp("Hunter", name) == 0 || strcmp("Jockey", name) == 0 || strcmp("Smoker", name) == 0 || strcmp("Spitter", name) == 0 || strcmp("Witch", name) == 0 || strcmp("Tank", name) == 0 ;
}

int CGlobal_GameUtil::getSpecialClassIdByName(const char* name) {
	if(strcmp("Boomer", name) == 0) {
		return Boomer;
	} else if(strcmp("Charger", name) == 0 ) {
		return Charger;
	} else if(strcmp("Hunter", name) == 0 ) {
		return Hunter;
	} else if(strcmp("Jockey", name) == 0 ) {
		return Jockey;
	} else if(strcmp("Smoker", name) == 0 ) {
		return Smoker;
	} else if(strcmp("Spitter", name) == 0 ) {
		return Spitter;
	} else if(strcmp("Witch", name) == 0 ) {
		return Witch;
	} else if(strcmp("Tank", name) == 0 ) {
		return Tank;
	} 
	return INVALID;
}

void CGlobal_GameUtil::backupCounter() {
	Necola_BackupCounter::weapon_first = Necola_Counter::weapon_first;
	Necola_BackupCounter::weapon_second = Necola_Counter::weapon_second;
	
	Necola_BackupCounter::first = Necola_Counter::first;
	Necola_BackupCounter::first_commons = Necola_Counter::first_commons;
	Necola_BackupCounter::first_commons_headshot = Necola_Counter::first_commons_headshot;
	Necola_BackupCounter::first_special_headshot = Necola_Counter::first_special_headshot;
	
	Necola_BackupCounter::first_boomer = Necola_Counter::first_boomer;
	Necola_BackupCounter::first_charger = Necola_Counter::first_charger;
	Necola_BackupCounter::first_hunter = Necola_Counter::first_hunter;
	Necola_BackupCounter::first_jockey = Necola_Counter::first_jockey;
	Necola_BackupCounter::first_smoker = Necola_Counter::first_smoker;
	Necola_BackupCounter::first_spitter = Necola_Counter::first_spitter;
	Necola_BackupCounter::first_tank = Necola_Counter::first_tank;
	Necola_BackupCounter::first_witch = Necola_Counter::first_witch;
	
	Necola_BackupCounter::second = Necola_Counter::second;
	Necola_BackupCounter::second_commons = Necola_Counter::second_commons;
	Necola_BackupCounter::second_commons_headshot = Necola_Counter::second_commons_headshot;
	Necola_BackupCounter::second_special_headshot = Necola_Counter::second_special_headshot;
	
	Necola_BackupCounter::second_boomer = Necola_Counter::second_boomer;
	Necola_BackupCounter::second_charger = Necola_Counter::second_charger;
	Necola_BackupCounter::second_hunter = Necola_Counter::second_hunter;
	Necola_BackupCounter::second_jockey = Necola_Counter::second_jockey;
	Necola_BackupCounter::second_smoker = Necola_Counter::second_smoker;
	Necola_BackupCounter::second_spitter = Necola_Counter::second_spitter;
	Necola_BackupCounter::second_tank = Necola_Counter::second_tank;
	Necola_BackupCounter::second_witch = Necola_Counter::second_witch;
}

void CGlobal_GameUtil::restoreCounter() {
	Necola_Counter::weapon_first = Necola_BackupCounter::weapon_first;
	Necola_Counter::weapon_second = Necola_BackupCounter::weapon_second;

	Necola_Counter::first = Necola_BackupCounter::first;
	Necola_Counter::first_commons = Necola_BackupCounter::first_commons;
	Necola_Counter::first_commons_headshot = Necola_BackupCounter::first_commons_headshot;
	Necola_Counter::first_special_headshot = Necola_BackupCounter::first_special_headshot;

	Necola_Counter::first_boomer = Necola_BackupCounter::first_boomer;
	Necola_Counter::first_charger = Necola_BackupCounter::first_charger;
	Necola_Counter::first_hunter = Necola_BackupCounter::first_hunter;
	Necola_Counter::first_jockey = Necola_BackupCounter::first_jockey;
	Necola_Counter::first_smoker = Necola_BackupCounter::first_smoker;
	Necola_Counter::first_spitter = Necola_BackupCounter::first_spitter;
	Necola_Counter::first_tank = Necola_BackupCounter::first_tank;
	Necola_Counter::first_witch = Necola_BackupCounter::first_witch;

	Necola_Counter::second = Necola_BackupCounter::second;
	Necola_Counter::second_commons = Necola_BackupCounter::second_commons;
	Necola_Counter::second_commons_headshot = Necola_BackupCounter::second_commons_headshot;
	Necola_Counter::second_special_headshot = Necola_BackupCounter::second_special_headshot;

	Necola_Counter::second_boomer = Necola_BackupCounter::second_boomer;
	Necola_Counter::second_charger = Necola_BackupCounter::second_charger;
	Necola_Counter::second_hunter = Necola_BackupCounter::second_hunter;
	Necola_Counter::second_jockey = Necola_BackupCounter::second_jockey;
	Necola_Counter::second_smoker = Necola_BackupCounter::second_smoker;
	Necola_Counter::second_spitter = Necola_BackupCounter::second_spitter;
	Necola_Counter::second_tank = Necola_BackupCounter::second_tank;
	Necola_Counter::second_witch = Necola_BackupCounter::second_witch;
}