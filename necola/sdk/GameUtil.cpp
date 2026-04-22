#include "./utils/GameUtil.h"

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

bool CGlobal_GameUtil::isExtraWeapon(int weaponId) {
	return  weaponId == WEAPON_MOLOTOV || weaponId == WEAPON_PIPEBOMB || weaponId == WEAPON_VOMITJAR
		|| weaponId == WEAPON_FIRSTAID_KIT || weaponId == WEAPON_DEFIBRILLATOR 
		|| weaponId == WEAPON_UPGRADEPACK_INCENDIARY || weaponId == WEAPON_UPGRADEPACK_EXPLOSIVE
		|| weaponId == WEAPON_PAINPILLS || weaponId == WEAPON_ADRENALINE;
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
	} else if(strcmp("shotgun_spas", weaponStr) == 0) {
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

bool CGlobal_GameUtil::weaponIsSlot(int weaponClassId, int slot) {
	bool flag = false;
	switch(weaponClassId) {
		case CSubMachinegun:
		case CPumpShotgun:
		case CAutoShotgun:
		case CAssaultRifle:
		case CSniperRifle:
		case CSMG_Silenced:
		case CShotgun_Chrome:
		case CRifle_Desert:
		case CSniper_Military:
		case CShotgun_SPAS:
		case CGrenadeLauncher:
		case CRifle_AK47:
		case CSMG_MP5:
		case CRifle_SG552:
		case CSniper_AWP:
		case CSniper_Scout:
		case CRifle_M60:
			{
				if(slot == 0) {
					flag = true;
				}
				break;
			}
			
		case CPistol:
		case CMagnumPistol:
		case CTerrorMeleeWeapon:
		case CChainsaw:
			{
				if(slot == 1) {
					flag = true;
				}
				break;
			}
		case CFirstAidKit:
		case CItemDefibrillator:
		case CItemUpgradePackIncendiary:
		case CItemUpgradePackExplosive:
			{
				if(slot == 2) {
					flag = true;
				}
				break;
			}
		case CMolotov:
		case CPipeBomb:
		case CItem_VomitJar:
			{
				if(slot == 3) {
					flag = true;
				}
				break;
			}
		case CPainPills:
		case CItem_Adrenaline:
			{
				if(slot == 4) {
					flag = true;
				}
				break;
			}
		default:
			break;

	}
	return flag;
}

/*
int CGlobal_GameUtil::getWeaponIDWithModelName(const char* modelName) {
	if(strstr(modelName, "v_bat") || strstr(modelName, "w_bat")){
		return NECOLA_WEAPON_MELEE_BAT;
	} else if(strstr(modelName, "_cricket_bat")) {
		return NECOLA_WEAPON_MELEE_CRICKETBAT;
	} else if(strstr(modelName, "_crowbar")) {
		return NECOLA_WEAPON_MELEE_CROWBAR;
	} else if(strstr(modelName, "_electric_guitar")) {
		return NECOLA_WEAPON_MELEE_GUITAR;
	} else if(strstr(modelName, "_fireaxe")) {
		return NECOLA_WEAPON_MELEE_FIREAXE;
	} else if(strstr(modelName, "_frying_pan")) {
		return NECOLA_WEAPON_MELEE_PAN;
	} else if(strstr(modelName, "_golfclub")) {
		return NECOLA_WEAPON_MELEE_GOLFGLUB;
	} else if(strstr(modelName, "_katana")) {
		return NECOLA_WEAPON_MELEE_KATANA;
	} else if(strstr(modelName, "_knife_t")) {
		return NECOLA_WEAPON_MELEE_KNIFE;
	} else if(strstr(modelName, "_machete")) {
		return NECOLA_WEAPON_MELEE_MACHETE;
	} else if(strstr(modelName, "_pitchfork")) {
		return NECOLA_WEAPON_MELEE_PITCHFORK;
	} else if(strstr(modelName, "_riotshield")) {
		return NECOLA_WEAPON_MELEE_SHIELD;
	} else if(strstr(modelName, "_shovel")) {
		return NECOLA_WEAPON_MELEE_SHOVEL;
	} else if(strstr(modelName, "_tonfa")) {
		return NECOLA_WEAPON_MELEE_TONFA;
	} 
	return -1;
}
*/
int CGlobal_GameUtil::getWeaponIDWithSubtype(const char* modelName) {
	if(strstr(modelName, "v_bat") || strstr(modelName, "w_bat")){
		return NECOLA_WEAPON_MELEE_BAT;
	} else if(strstr(modelName, "_cricket_bat")) {
		return NECOLA_WEAPON_MELEE_CRICKETBAT;
	} else if(strstr(modelName, "_crowbar")) {
		return NECOLA_WEAPON_MELEE_CROWBAR;
	} else if(strstr(modelName, "_electric_guitar")) {
		return NECOLA_WEAPON_MELEE_GUITAR;
	} else if(strstr(modelName, "_fireaxe")) {
		return NECOLA_WEAPON_MELEE_FIREAXE;
	} else if(strstr(modelName, "_frying_pan")) {
		return NECOLA_WEAPON_MELEE_PAN;
	} else if(strstr(modelName, "_golfclub")) {
		return NECOLA_WEAPON_MELEE_GOLFGLUB;
	} else if(strstr(modelName, "_katana")) {
		return NECOLA_WEAPON_MELEE_KATANA;
	} else if(strstr(modelName, "_knife_t")) {
		return NECOLA_WEAPON_MELEE_KNIFE;
	} else if(strstr(modelName, "_machete")) {
		return NECOLA_WEAPON_MELEE_MACHETE;
	} else if(strstr(modelName, "_pitchfork")) {
		return NECOLA_WEAPON_MELEE_PITCHFORK;
	} else if(strstr(modelName, "_riotshield")) {
		return NECOLA_WEAPON_MELEE_SHIELD;
	} else if(strstr(modelName, "_shovel")) {
		return NECOLA_WEAPON_MELEE_SHOVEL;
	} else if(strstr(modelName, "_tonfa")) {
		return NECOLA_WEAPON_MELEE_TONFA;
	} 
	return -1;
}


int CGlobal_GameUtil::getWeaponIDWithViewModelSubtype(const char* modelName) {
	if(strstr(modelName, "v_bat") || strstr(modelName, "v_bat_ncl") ){
		return NECOLA_WEAPON_MELEE_BAT;
	} else if(strstr(modelName, "v_cricket_bat") || strstr(modelName, "v_cricket_bat_ncl")) {
		return NECOLA_WEAPON_MELEE_CRICKETBAT;
	} else if(strstr(modelName, "v_crowbar") || strstr(modelName, "v_crowbar_ncl")) {
		return NECOLA_WEAPON_MELEE_CROWBAR;
	} else if(strstr(modelName, "v_electric_guitar") || strstr(modelName, "v_electric_guitar_ncl")) {
		return NECOLA_WEAPON_MELEE_GUITAR;
	} else if(strstr(modelName, "v_fireaxe") || strstr(modelName, "v_fireaxe_ncl")) {
		return NECOLA_WEAPON_MELEE_FIREAXE;
	} else if(strstr(modelName, "v_frying_pan") || strstr(modelName, "v_frying_pan_ncl")) {
		return NECOLA_WEAPON_MELEE_PAN;
	} else if(strstr(modelName, "v_golfclub") || strstr(modelName, "v_golfclub_ncl")) {
		return NECOLA_WEAPON_MELEE_GOLFGLUB;
	} else if(strstr(modelName, "v_katana") || strstr(modelName, "v_katana_ncl")) {
		return NECOLA_WEAPON_MELEE_KATANA;
	} else if(strstr(modelName, "v_knife_t") || strstr(modelName, "v_knife_t_ncl")) {
		return NECOLA_WEAPON_MELEE_KNIFE;
	} else if(strstr(modelName, "v_machete") || strstr(modelName, "v_machete_ncl")) {
		return NECOLA_WEAPON_MELEE_MACHETE;
	} else if(strstr(modelName, "v_pitchfork") || strstr(modelName, "v_pitchfork_ncl")) {
		return NECOLA_WEAPON_MELEE_PITCHFORK;
	} else if(strstr(modelName, "v_riotshield") || strstr(modelName, "v_riotshield_ncl")) {
		return NECOLA_WEAPON_MELEE_SHIELD;
	} else if(strstr(modelName, "v_shovel") || strstr(modelName, "v_shovel_ncl")) {
		return NECOLA_WEAPON_MELEE_SHOVEL;
	} else if(strstr(modelName, "v_tonfa") || strstr(modelName, "v_tonfa_ncl")) {
		return NECOLA_WEAPON_MELEE_TONFA;
	} 
	return -1;
}


int CGlobal_GameUtil::getWeaponIDWithWModel(const char* modelName) {
	// å¿«é€Ÿæ£€æŸ¥å‰å‡ ä¸ªå­—ç¬¦æ¥ç¼©å°èŒƒå›?
	if (modelName[0] != 'm' || modelName[1] != 'o' || modelName[2] != 'd' || 
		modelName[3] != 'e' || modelName[4] != 'l' || modelName[5] != 's' || 
		modelName[6] != '/') {
		return -1;
	}

	const char* path = modelName + 7; // è·³è¿‡ "models/"
	
	// æ ¹æ®å­ç›®å½•å¿«é€Ÿåˆ†ç±?
	if (strncmp(path, "weapons/melee/", 14) == 0) {
		const char* weapon = path + 14; // è·³è¿‡ "weapons/melee/"
		
		// é€ä¸ªå­—ç¬¦æ¯”è¾ƒï¼Œé¿å…strcmpçš„å®Œæ•´æ‰«æ?
		if (weapon[0] == 'w' && weapon[1] == '_') {
			const char* name = weapon + 2;
			
			// æŒ‰é¦–å­—æ¯åˆ†ç»„æ¯”è¾ƒ
			switch (name[0]) {
				case 't':
					if (strncmp(name, "tonfa", 5) == 0) return NECOLA_WEAPON_MELEE_TONFA;
					break;
				case 's':
					if (strncmp(name, "shovel", 6) == 0) return NECOLA_WEAPON_MELEE_SHOVEL;
					break;
				case 'r':
					if (strncmp(name, "riotshield", 10) == 0) return NECOLA_WEAPON_MELEE_SHIELD;
					break;
				case 'p':
					if (strncmp(name, "pitchfork", 9) == 0) return NECOLA_WEAPON_MELEE_PITCHFORK;
					break;
				case 'm':
					if (strncmp(name, "machete", 7) == 0) return NECOLA_WEAPON_MELEE_MACHETE;
					break;
				case 'k':
					if (strncmp(name, "katana", 6) == 0) return NECOLA_WEAPON_MELEE_KATANA;
					break;
				case 'g':
					if (strncmp(name, "golfclub", 8) == 0) return NECOLA_WEAPON_MELEE_GOLFGLUB;
					break;
				case 'f':
					if (name[1] == 'r') {
						if (strncmp(name, "frying_pan", 10) == 0) return NECOLA_WEAPON_MELEE_PAN;
					} else if (name[1] == 'i') {
						if (strncmp(name, "fireaxe", 7) == 0) return NECOLA_WEAPON_MELEE_FIREAXE;
					}
					break;
				case 'e':
					if (strncmp(name, "electric_guitar", 15) == 0) return NECOLA_WEAPON_MELEE_GUITAR;
					break;
				case 'c':
					if (name[1] == 'r') {
						if (strncmp(name, "crowbar", 7) == 0) return NECOLA_WEAPON_MELEE_CROWBAR;
					} else if (name[1] == 'r') {
						if (strncmp(name, "cricket_bat", 11) == 0) return NECOLA_WEAPON_MELEE_CRICKETBAT;
					} else if (name[1] == 'h') {
						if (strncmp(name, "chainsaw", 8) == 0) return NECOLA_WEAPON_CHAINSAW;
					}
					break;
				case 'b':
					if (strncmp(name, "bat", 3) == 0) return NECOLA_WEAPON_MELEE_BAT;
					break;
			}
		}
	}
	else if (strncmp(path, "w_models/weapons/", 17) == 0) {
		const char* weapon = path + 17; // è·³è¿‡ "w_models/weapons/"
		
		if (weapon[0] == 'w' && weapon[1] == '_') {
			const char* name = weapon + 2;
			
			// æŒ‰æ­¦å™¨ç±»åž‹åˆ†ç»?
			if (strncmp(name, "knife_t", 7) == 0 ) return NECOLA_WEAPON_MELEE_KNIFE;
			if (strncmp(name, "m60", 3) == 0 ) return NECOLA_WEAPON_M60;
			
			// ç‹™å‡»æž?
			if (strncmp(name, "sniper_", 7) == 0) {
				const char* sniper = name + 7;
				if (strncmp(sniper, "scout", 5) == 0) return NECOLA_WEAPON_SCOUT;
				if (strncmp(sniper, "awp", 3) == 0) return NECOLA_WEAPON_AWP;
				if (strncmp(sniper, "military", 8) == 0) return NECOLA_WEAPON_MILITARY_SNIPER;
				if (strncmp(sniper, "mini14", 6) == 0) return NECOLA_WEAPON_HUNTING_RIFLE;
			}
			
			// æ­¥æžª
			if (strncmp(name, "rifle_", 6) == 0) {
				const char* rifle = name + 6;
				if (strncmp(rifle, "sg552", 5) == 0) return NECOLA_WEAPON_SSG552;
				if (strncmp(rifle, "ak47", 4) == 0) return NECOLA_WEAPON_AK47;
				if (strncmp(rifle, "m16a2", 5) == 0) return NECOLA_WEAPON_M16A1;
			}
			
			// SMG
			if (strncmp(name, "smg_", 4) == 0) {
				const char* smg = name + 4;
				if (strncmp(smg, "mp5", 3) == 0) return NECOLA_WEAPON_MP5;
				if (strncmp(smg, "a", 1) == 0) return NECOLA_WEAPON_MAC10;
				if (strncmp(smg, "uzi", 3) == 0) return NECOLA_WEAPON_UZI;
			}
			
			// æ‰‹æžª
			if (strncmp(name, "pistol_", 7) == 0) {
				return NECOLA_WEAPON_PISTOL;
				// const char* pistol = name + 7;
				// if (strcmp(pistol, "b.mdl") == 0) return 42;
				// if (strcmp(pistol, "a.mdl") == 0) return 43;
				// if (strcmp(pistol, "a_dual.mdl") == 0) return 44;
			}
			
			// éœ°å¼¹æž?
			if (strncmp(name, "shotgun", 7) == 0) {
				const char* shotgun = name + 7;
				if (strncmp(shotgun, "_spas", 5) == 0) {
					return NECOLA_WEAPON_SPAS;
				} else {
					return NECOLA_WEAPON_CHROME_SHOTGUN;
				}
				//if (shotgun[0] == '\0' || strcmp(shotgun, ".mdl") == 0) return NECOLA_WEAPON_CHROME_SHOTGUN;
				
			}
			if (strncmp(name, "pumpshotgun", 11) == 0) return NECOLA_WEAPON_PUMP_SHOTGUN;
			if (strncmp(name, "autoshot_m4super", 16) == 0) return NECOLA_WEAPON_AUTO_SHOTGUN;
			
			// ç‰¹æ®Šæ­¦å™¨
			if (strncmp(name, "desert_eagle", 12) == 0) return NECOLA_WEAPON_DEAGLE;
			if (strncmp(name, "desert_rifle", 12) == 0) return NECOLA_WEAPON_SCAR;
			if (strncmp(name, "grenade_launcher", 16) == 0) return NECOLA_WEAPON_GRENADE_LAUNCHER;
			
			// è£…å¤‡ç‰©å“
			if (strncmp(name, "eq_", 3) == 0) {
				const char* eq = name + 3;
				if (strncmp(eq, "explosive_ammopack", 18) == 0) return NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE;
				if (strncmp(eq, "incendiary_ammopack", 19) == 0) return NECOLA_WEAPON_UPGRADEPACK_INCENDIARY;
				if (strncmp(eq, "bile_flask", 10) == 0) return NECOLA_WEAPON_VOMITJAR;
				if (strncmp(eq, "defibrillator", 13) == 0) return NECOLA_WEAPON_DEFIBRILLATOR;
				if (strncmp(eq, "adrenaline", 14) == 0) return NECOLA_WEAPON_ADRENALINE;
				if (strncmp(eq, "painpills", 9) == 0) return NECOLA_WEAPON_PAINPILLS;
				if (strncmp(eq, "pipebomb", 8) == 0) return NECOLA_WEAPON_PIPEBOMB;
				if (strncmp(eq, "molotov", 7) == 0) return NECOLA_WEAPON_MOLOTOV;
				if (strncmp(eq, "medkit", 6) == 0) return NECOLA_WEAPON_FIRSTAID_KIT;
			}
		}
	}
	
	return -1;


}
/*
int CGlobal_GameUtil::getWeaponSlotByItemName(const char* itemName) {
	if(strstr(itemName, "rifle") || strstr(itemName, "shotgun") || strstr(itemName, "sniper") || strstr(itemName, "smg") || strstr(itemName, "grenade_launcher")){
		return 0;
	} else if(strstr(itemName, "pistol") || strstr(itemName, "melee") || strstr(itemName, "chainsaw")) {
		return 1;
	} else if(strstr(itemName, "upgradepack") || strstr(itemName, "defibrillator") || strstr(itemName, "first_aid_kit")) {
		return 3;
	} else if(strstr(itemName, "molotov") || strstr(itemName, "pipe_bomb") || strstr(itemName, "vomitjar")) {
		return 2;
	} else if(strstr(itemName, "pain_pills") || strstr(itemName, "adrenaline")) {
		return 4;
	} 
	return -1;
}
*/

int CGlobal_GameUtil::getWeaponIDByWeaponName(const char* itemName) {
	if(strcmp(itemName, "weapon_pistol") == 0) {
		return NECOLA_WEAPON_PISTOL;
	} else if(strcmp(itemName, "weapon_pistol_magnum") == 0) {
		return NECOLA_WEAPON_DEAGLE;
	} else if(strcmp(itemName, "weapon_chainsaw") == 0) {
		return NECOLA_WEAPON_CHAINSAW;
	} else if(strcmp(itemName, "weapon_melee") == 0) {
		return NECOLA_WEAPON_MELEE;
	} else if(strcmp(itemName, "weapon_smg") == 0) {
		return NECOLA_WEAPON_UZI;
	} else if(strcmp(itemName, "weapon_smg_silenced") == 0) {
		return NECOLA_WEAPON_MAC10;
	} else if(strcmp(itemName, "weapon_smg_mp5") == 0) {
		return NECOLA_WEAPON_MP5;
	} else if(strcmp(itemName, "weapon_pumpshotgun") == 0) {
		return NECOLA_WEAPON_PUMP_SHOTGUN;
	} else if(strcmp(itemName, "weapon_shotgun_chrome") == 0) {
		return NECOLA_WEAPON_CHROME_SHOTGUN;
	} else if(strcmp(itemName, "weapon_autoshotgun") == 0) {
		return NECOLA_WEAPON_AUTO_SHOTGUN;
	} else if(strcmp(itemName, "weapon_shotgun_spas") == 0) {
		return NECOLA_WEAPON_SPAS;
	} else if(strcmp(itemName, "weapon_rifle") == 0) {
		return NECOLA_WEAPON_M16A1;
	} else if(strcmp(itemName, "weapon_rifle_desert") == 0) {
		return NECOLA_WEAPON_SCAR;
	} else if(strcmp(itemName, "weapon_rifle_ak47") == 0) {
		return NECOLA_WEAPON_AK47;
	} else if(strcmp(itemName, "weapon_rifle_sg552") == 0) {
		return NECOLA_WEAPON_SSG552;
	} else if(strcmp(itemName, "weapon_rifle_m60") == 0) {
		return NECOLA_WEAPON_M60;
	} else if(strcmp(itemName, "weapon_hunting_rifle") == 0) {
		return NECOLA_WEAPON_HUNTING_RIFLE;
	} else if(strcmp(itemName, "weapon_sniper_military") == 0) {
		return NECOLA_WEAPON_MILITARY_SNIPER;
	} else if(strcmp(itemName, "weapon_sniper_scout") == 0) {
		return NECOLA_WEAPON_SCOUT;
	} else if(strcmp(itemName, "weapon_sniper_awp") == 0) {
		return NECOLA_WEAPON_AWP;
	} else if(strcmp(itemName, "weapon_grenade_launcher") == 0) {
		return NECOLA_WEAPON_GRENADE_LAUNCHER;
	} else if(strcmp(itemName, "weapon_first_aid_kit") == 0) {
		return NECOLA_WEAPON_FIRSTAID_KIT;
	} else if(strcmp(itemName, "weapon_defibrillator") == 0) {
		return NECOLA_WEAPON_DEFIBRILLATOR;
	} else if(strcmp(itemName, "weapon_upgradepack_explosive") == 0) {
		return NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE;
	} else if(strcmp(itemName, "weapon_upgradepack_incendiary") == 0) {
		return NECOLA_WEAPON_UPGRADEPACK_INCENDIARY;
	} else if(strcmp(itemName, "weapon_pipe_bomb") == 0) {
		return NECOLA_WEAPON_PIPEBOMB;
	} else if(strcmp(itemName, "weapon_molotov") == 0) {
		return NECOLA_WEAPON_MOLOTOV;
	} else if(strcmp(itemName, "weapon_vomitjar") == 0) {
		return NECOLA_WEAPON_VOMITJAR;
	} else if(strcmp(itemName, "weapon_pain_pills") == 0) {
		return NECOLA_WEAPON_PAINPILLS;
	} else if(strcmp(itemName, "weapon_adrenaline") == 0) {
		return NECOLA_WEAPON_ADRENALINE;
	} else if(strcmp(itemName, "weapon_fireworkcrate") == 0) {
		return NECOLA_WEAPON_FIREWORK;
	} else if(strcmp(itemName, "weapon_oxygentank") == 0) {
		return NECOLA_WEAPON_OXYGEN_TANK;
	} else if(strcmp(itemName, "weapon_propanetank") == 0) {
		return NECOLA_WEAPON_PROPANE_TANK;
	} else if(strcmp(itemName, "weapon_gascan") == 0) {
		return NECOLA_WEAPON_GASCAN;
	} else if(strcmp(itemName, "weapon_cola_bottles") == 0) {
		return NECOLA_WEAPON_COLA;
	} else if(strcmp(itemName, "weapon_gnome") == 0) {
		return NECOLA_WEAPON_GNOME;
	}
	return -1;
}


int CGlobal_GameUtil::entityClassID2WeaponID(int EntClassID) {
	int weaponId = -1;
	switch(EntClassID) {
		case CPistol:
			weaponId = NECOLA_WEAPON_PISTOL;
			break;
		case CSubMachinegun:
			weaponId = NECOLA_WEAPON_UZI;
			break;
		case CPumpShotgun:
			weaponId = NECOLA_WEAPON_PUMP_SHOTGUN;
			break;
		case CAutoShotgun:
			weaponId = NECOLA_WEAPON_AUTO_SHOTGUN;
			break;
		case CAssaultRifle:
			weaponId = NECOLA_WEAPON_M16A1;
			break;
		case CSniperRifle:
			weaponId = NECOLA_WEAPON_HUNTING_RIFLE;
			break;
		case CSMG_Silenced:
			weaponId = NECOLA_WEAPON_MAC10;
			break;
		case CShotgun_Chrome:
			weaponId = NECOLA_WEAPON_CHROME_SHOTGUN;
			break;
		case CRifle_Desert:
			weaponId = NECOLA_WEAPON_SCAR;
			break;
		case CSniper_Military:
			weaponId = NECOLA_WEAPON_MILITARY_SNIPER;
			break;
		case CShotgun_SPAS:
			weaponId = NECOLA_WEAPON_SPAS;
			break;
		case CFirstAidKit:
			weaponId = NECOLA_WEAPON_FIRSTAID_KIT;
			break;
		case CMolotov:
			weaponId = NECOLA_WEAPON_MOLOTOV;
			break;
		case CPipeBomb:
			weaponId = NECOLA_WEAPON_PIPEBOMB;
			break;
		case CPainPills:
			weaponId = NECOLA_WEAPON_PAINPILLS;
			break;
		case CGasCan:
			weaponId = NECOLA_WEAPON_GASCAN;
			break;
		case CPropaneTank:
			weaponId = NECOLA_WEAPON_PROPANE_TANK;
			break;
		case COxygenTank:
			weaponId = NECOLA_WEAPON_OXYGEN_TANK;
			break;
		//Melee
		case CTerrorMeleeWeapon:
			weaponId = NECOLA_WEAPON_MELEE;
			break;
		case CChainsaw:
			weaponId = NECOLA_WEAPON_CHAINSAW;
			break;
		case CGrenadeLauncher:
			weaponId = NECOLA_WEAPON_GRENADE_LAUNCHER;
			break;
		case CItem_Adrenaline:
			weaponId = NECOLA_WEAPON_ADRENALINE;
			break;
		case CItemDefibrillator:
			weaponId = NECOLA_WEAPON_DEFIBRILLATOR;
			break;
		case CItem_VomitJar:
			weaponId = NECOLA_WEAPON_VOMITJAR;
			break;
		case CRifle_AK47:
			weaponId = NECOLA_WEAPON_AK47;
			break;
		case CColaBottles:
			weaponId = NECOLA_WEAPON_COLA;
			break;
		case CGnome:
			weaponId = NECOLA_WEAPON_GNOME;
			break;
		case CFireworkCrate:
			weaponId = NECOLA_WEAPON_FIREWORK;
			break;
		case CItemUpgradePackIncendiary:
			weaponId = NECOLA_WEAPON_UPGRADEPACK_INCENDIARY;
			break;
		case CItemUpgradePackExplosive:
			weaponId = NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE;
			break;
		case CMagnumPistol:
			weaponId = NECOLA_WEAPON_DEAGLE;
			break;
		case CSMG_MP5:
			weaponId = NECOLA_WEAPON_MP5;
			break;
		case CRifle_SG552:
			weaponId = NECOLA_WEAPON_SSG552;
			break;
		case CSniper_AWP:
			weaponId = NECOLA_WEAPON_AWP;
			break;
		case CSniper_Scout:
			weaponId = NECOLA_WEAPON_SCOUT;
			break;
		case CRifle_M60:
			weaponId = NECOLA_WEAPON_M60;
			break;
	}
	return weaponId;
}


const char* CGlobal_GameUtil::convertToNecolaModelName(const char* sourceModelName, int poly) {
	if( poly == 0 ){
		return sourceModelName;
	}
	const char* mdlPos = strstr(sourceModelName, ".mdl");
	if (mdlPos == nullptr) {
		size_t len = strlen(sourceModelName);
		char* result = new char[len + 1];
		strcpy(result, sourceModelName);
		return result;
	}
	size_t prefixLen = mdlPos - sourceModelName;
	char insertStr[8] = {0};

	snprintf(insertStr, sizeof(insertStr), "_ncl%d", poly);
	size_t insertLen = strlen(insertStr);
	size_t suffixLen = strlen(mdlPos);
	size_t totalLen = prefixLen + insertLen + suffixLen;
	char* result = new char[totalLen + 1];
	strncpy(result, sourceModelName, prefixLen);
	result[prefixLen] = '\0';
	strcat(result, insertStr);
	strcat(result, mdlPos);
	return result;
}


void CGlobal_GameUtil::convertNecolaToSzName(const char* modelname, const char* szNameTmp, char* insertBuffer) {
		if (insertBuffer == nullptr) {
		return;
	}
	insertBuffer[0] = '\0';
	if (szNameTmp == nullptr || szNameTmp[0] == '\0') {
		return;
	}
	const char* nclPos = strstr(modelname, "_ncl");
	if (nclPos == nullptr) {
		strncpy(insertBuffer, szNameTmp, 63);
		insertBuffer[63] = '\0';
		return;
	}

	const char* numStart = nclPos + 4; 
	if (!isdigit(*numStart)) {
		strncpy(insertBuffer, szNameTmp, 63);
		insertBuffer[63] = '\0';
		return;
	}

	char polyStr[16] = {0};
	int i = 0;
	while (isdigit(numStart[i]) && i < 15) {
		polyStr[i] = numStart[i];
		i++;
	}
	polyStr[i] = '\0';
	const char* mdlPos = strstr(szNameTmp, ".mdl");
	if (mdlPos == nullptr) {
		strncpy(insertBuffer, szNameTmp, 63);
		insertBuffer[63] = '\0';
		return;
	}

	size_t prefixLen = mdlPos - szNameTmp;

	char insertStr[32] = {0};
	snprintf(insertStr, sizeof(insertStr), "_ncl%s", polyStr);
	size_t insertLen = strlen(insertStr);
	size_t suffixLen = strlen(mdlPos);

	size_t totalLen = prefixLen + insertLen + suffixLen;
	if (totalLen > 63) {
		// è®¡ç®—éœ€è¦æˆªæ–­çš„é•¿åº¦
		size_t overflow = totalLen - 63;
		if (prefixLen > overflow) {
			prefixLen -= overflow;
		} else {
			size_t neededCut = overflow - prefixLen;
			if (insertLen > neededCut) {
				insertLen -= neededCut;
				insertStr[insertLen] = '\0';
			} else {
			
				neededCut -= insertLen;
				if (suffixLen > neededCut + 4) { 
					suffixLen -= neededCut;
				} else {
	
					strncpy(insertBuffer, szNameTmp, 63);
					insertBuffer[63] = '\0';
					return;
				}
			}
			prefixLen = 0; 
		}
	}
	

	strncpy(insertBuffer, szNameTmp, prefixLen);
	insertBuffer[prefixLen] = '\0';

	strncat(insertBuffer, insertStr, 63 - strlen(insertBuffer));

	strncat(insertBuffer, mdlPos, 63 - strlen(insertBuffer));
	insertBuffer[63] = '\0';
}


std::string CGlobal_GameUtil::convertFilenameWithRegex(const char* pFilename) {
	if (!pFilename) return "";
	std::string filename(pFilename);
	std::regex pattern(R"((.*)_ncl(\d+)\.phy$)");
	std::smatch matches;
	
	if (std::regex_match(filename, matches, pattern)) {
		return matches[1].str() + ".phy";
	}
	return filename;
}


bool CGlobal_GameUtil::hasAddonModelWeapon(int entityClass) {
	if(   entityClass == CFirstAidKit ||  entityClass == CItemDefibrillator 
		|| entityClass == CItemUpgradePackIncendiary || entityClass == CItemUpgradePackExplosive
		|| entityClass == CFireworkCrate || entityClass == CColaBottles || entityClass == CGasCan 
		|| entityClass == CGnome  ||  entityClass == COxygenTank || entityClass == CPropaneTank
		|| entityClass == CMolotov || entityClass == CItem_VomitJar || entityClass == CPipeBomb 
		|| entityClass == CItem_Adrenaline || entityClass == CPainPills 
		) {
		// need check
		// || entityClass == CTerrorMeleeWeapon
		return true;
	}
	return false;
}


bool CGlobal_GameUtil::isTracerChangerSupport(int weaponClassId) {
	bool flag = false;
	switch(weaponClassId) {
		case CSubMachinegun:
		case CPumpShotgun:
		case CAutoShotgun:
		case CAssaultRifle:
		case CSniperRifle:
		case CSMG_Silenced:
		case CShotgun_Chrome:
		case CRifle_Desert:
		case CSniper_Military:
		case CShotgun_SPAS:
		case CRifle_AK47:
		case CSMG_MP5:
		case CRifle_SG552:
		case CSniper_AWP:
		case CSniper_Scout:
		case CRifle_M60:
		case CPistol:
		case CMagnumPistol:
			flag = true;
			break;
		}
	return flag;
}

bool CGlobal_GameUtil::isTracerChangerSupportWeaponID(int weaponId) {
	bool flag = false;
	switch(weaponId) {
		case NECOLA_WEAPON_UZI:
		case NECOLA_WEAPON_PUMP_SHOTGUN:
		case NECOLA_WEAPON_AUTO_SHOTGUN:
		case NECOLA_WEAPON_M16A1:
		case NECOLA_WEAPON_HUNTING_RIFLE:
		case NECOLA_WEAPON_MAC10:
		case NECOLA_WEAPON_CHROME_SHOTGUN:
		case NECOLA_WEAPON_SCAR:
		case NECOLA_WEAPON_MILITARY_SNIPER:
		case NECOLA_WEAPON_SPAS:
		case NECOLA_WEAPON_AK47:
		case NECOLA_WEAPON_MP5:
		case NECOLA_WEAPON_SSG552:
		case NECOLA_WEAPON_AWP:
		case NECOLA_WEAPON_SCOUT:
		case NECOLA_WEAPON_M60:
		case NECOLA_WEAPON_PISTOL:
		case NECOLA_WEAPON_DEAGLE:
			flag = true;
			break;
		}
	return flag;
}

const char* CGlobal_GameUtil::getWeaponTracerName(int weaponClassId) {
	if(weaponClassId == CSubMachinegun) {
		return "weapon_tracers_uzi";
	} else if(weaponClassId == CPumpShotgun) {
		return "weapon_tracers_pump";
	} else if(weaponClassId == CAutoShotgun) {
		return "weapon_tracers_auto";
	} else if(weaponClassId == CAssaultRifle) {
		return "weapon_tracers_m16";
	} else if(weaponClassId == CSniperRifle) {
		return "weapon_tracers_mini14";
	} else if(weaponClassId == CSMG_Silenced) {
		return "weapon_tracers_mac10";
	} else if(weaponClassId == CShotgun_Chrome) {
		return "weapon_tracers_chrome";
	} else if(weaponClassId == CRifle_Desert) {
		return "weapon_tracers_scar";
	} else if(weaponClassId == CSniper_Military) {
		return "weapon_tracers_military";
	} else if(weaponClassId == CShotgun_SPAS) {
		return "weapon_tracers_spas";
	} else if(weaponClassId == CRifle_AK47) {
		return "weapon_tracers_ak47";
	} else if(weaponClassId == CSMG_MP5) {
		return "weapon_tracers_mp5";
	} else if(weaponClassId == CRifle_SG552) {
		return "weapon_tracers_sg552";
	} else if(weaponClassId == CSniper_AWP) {
		return "weapon_tracers_awp";
	} else if(weaponClassId == CSniper_Scout) {
		return "weapon_tracers_scout";
	} else if(weaponClassId == CRifle_M60) {
		return "weapon_tracers_m60";
	} else if(weaponClassId == CPistol) {
		return "weapon_tracers_pistol";
	} else if(weaponClassId == CMagnumPistol) {
		return "weapon_tracers_magnum";
	} 
	return "";
}
const char* CGlobal_GameUtil::getWeaponTracerTypeName(int weaponClassId) {
	const char* rtName = "";
	switch(weaponClassId) {
		case CPistol:
		case CMagnumPistol:
			rtName = "weapon_tracers_pistol_s";
			break;
		case CSubMachinegun:
		case CSMG_Silenced:
		case CSMG_MP5:
			rtName = "weapon_tracers_smg";
			break;
		case CAssaultRifle:
		case CRifle_Desert:
		case CRifle_AK47:
		case CRifle_SG552:
		case CRifle_M60:
			rtName = "weapon_tracers_rifle";
			break;
		case CPumpShotgun:
		case CAutoShotgun:
		case CShotgun_Chrome:
		case CShotgun_SPAS:
			rtName = "weapon_tracers_shotgun";
			break;
		case CSniperRifle:
		case CSniper_Military:
		case CSniper_AWP:
		case CSniper_Scout:
			rtName = "weapon_tracers_sniper";
			break;
	}

	return rtName;
}



bool CGlobal_GameUtil::isSequenceModiferWeapon(int weaponID) {
	bool ret = false;
	switch(weaponID) {
		case NECOLA_WEAPON_PISTOL:
		case NECOLA_WEAPON_PISTOL_DUAL:
		case NECOLA_WEAPON_DEAGLE:
		case NECOLA_WEAPON_CHAINSAW:

		case NECOLA_WEAPON_UZI:
		case NECOLA_WEAPON_MAC10:
		case NECOLA_WEAPON_MP5:

		case NECOLA_WEAPON_PUMP_SHOTGUN:
		case NECOLA_WEAPON_AUTO_SHOTGUN:
		case NECOLA_WEAPON_CHROME_SHOTGUN:
		case NECOLA_WEAPON_SPAS:

		case NECOLA_WEAPON_M16A1:
		case NECOLA_WEAPON_SCAR:
		case NECOLA_WEAPON_AK47:
		case NECOLA_WEAPON_SSG552:
		case NECOLA_WEAPON_M60:

		case NECOLA_WEAPON_HUNTING_RIFLE:
		case NECOLA_WEAPON_MILITARY_SNIPER:
		case NECOLA_WEAPON_AWP:
		case NECOLA_WEAPON_SCOUT:

		case NECOLA_WEAPON_GRENADE_LAUNCHER:
			
		case NECOLA_WEAPON_MELEE_BAT:
		case NECOLA_WEAPON_MELEE_CRICKETBAT:
		case NECOLA_WEAPON_MELEE_CROWBAR:
		case NECOLA_WEAPON_MELEE_GUITAR:
		case NECOLA_WEAPON_MELEE_FIREAXE:
		case NECOLA_WEAPON_MELEE_PAN:
		case NECOLA_WEAPON_MELEE_GOLFGLUB:
		case NECOLA_WEAPON_MELEE_KATANA:
		case NECOLA_WEAPON_MELEE_KNIFE:
		case NECOLA_WEAPON_MELEE_MACHETE:
		case NECOLA_WEAPON_MELEE_PITCHFORK:
		case NECOLA_WEAPON_MELEE_SHIELD:
		case NECOLA_WEAPON_MELEE_SHOVEL:
		case NECOLA_WEAPON_MELEE_TONFA:
		
		case NECOLA_WEAPON_FIRSTAID_KIT:
		case NECOLA_WEAPON_DEFIBRILLATOR:
		case NECOLA_WEAPON_UPGRADEPACK_INCENDIARY:
		case NECOLA_WEAPON_UPGRADEPACK_EXPLOSIVE:

		case NECOLA_WEAPON_MOLOTOV:
		case NECOLA_WEAPON_PIPEBOMB:
		case NECOLA_WEAPON_VOMITJAR:

		case NECOLA_WEAPON_PAINPILLS:
		case NECOLA_WEAPON_ADRENALINE:
			ret = true;
			break;
	}
	return ret;
}

bool CGlobal_GameUtil::isNecolaActivity(int activity) {
	bool ret = false;
	switch(activity) {
		case ACT_VM_DEPLOY_LAYER:
		case ACT_VM_DEPLOY_PISTOL_LAYER:
		case ACT_VM_DEPLOY_DUAL_PISTOL_LAYER:
		case ACT_VM_RELOAD_LAYER:
		case ACT_VM_RELOAD_EMPTY_LAYER:
		case ACT_VM_MELEE_LAYER:
		case ACT_VM_RELOAD_LOOP_LAYER:
		case ACT_VM_RELOAD_END_LAYER:
		case ACT_VM_RELOAD_SNIPER_LAYER:
		case ACT_VM_DEPLOY_SNIPER_LAYER:
		case ACT_VM_MELEE_SNIPER_LAYER:
		case ACT_VM_DRAW:
		case ACT_VM_HITCENTER:
		case ACT_VM_HITLEFT:
		case ACT_VM_HITRIGHT:
		case ACT_VM_SECONDARYATTACK_LAYER:
		case ACT_VM_SHOOT_LAYER:
		case ACT_VM_DEPLOY_GASCAN:
		case ACT_VM_MELEE_GASCAN:
		// case ACT_VM_DEPLOY_GASCAN_LAYER:
		// case ACT_VM_MELEE_GASCAN_LAYER:
		case ACT_VM_PRIMARYATTACK_LAYER:
		case ACT_VM_DEPLOY_MOLOTOV_LAYER:
		case ACT_VM_PULLPIN_MOLOTOV_LAYER:
		case ACT_VM_THROW_MOLOTOV_LAYER:
		case ACT_VM_MELEE_MOLOTOV_LAYER:
		case ACT_VM_DEPLOY_PAINPILLS_LAYER:
		case ACT_VM_MELEE_PAINPILLS_LAYER:
		case ACT_VM_DEPLOY_PIPEBOMB_LAYER:
		case ACT_VM_PULLPIN_PIPEBOMB_LAYER:
		case ACT_VM_THROW_PIPEBOMB_LAYER:
		case ACT_VM_MELEE_PIPEBOMB_LAYER:
		case ACT_VM_SHOOT_SNIPER_LAYER:
		case ACT_VM_CANCEL_LAYER:
		case ACT_VM_SWINGHIT:
		case ACT_VM_SWINGHARD:
			ret = true;
			break;
	}
	return ret;
}

bool CGlobal_GameUtil::isParticleCustomWeaponFireSoundSample(const char* pSample) {
	return false;
}

int CGlobal_GameUtil::getNecolaWeaponIDWithSample(const char* pSample) {
if (!pSample || !pSample[0]) return 0;
	 
	const char* s = pSample;
	if (*s == ')') s++;

	// å¿«é€Ÿgunfireæ£€æŸ?
	if (!strstr(s, "gunfire/")) return 0;

	// æå–æ­¦å™¨ç±»åž‹
	const char* wpn = strstr(s, "weapons/");
	if (!wpn) return 0;

	const char* type = wpn + 8;  // è·³è¿‡"weapons/"

	// åŸºäºŽæ­¦å™¨ç±»åž‹çš„å¿«é€Ÿåˆ¤æ–?
	char c1 = type[0];
	char c2 = type[1];
	char c3 = type[2];

	// IDåˆ†é…ï¼ˆæ ¹æ®åŽŸå§‹é¡ºåºï¼‰ï¼?
	// 1-2: pistol (å?åŒæŒ)
	// 3: magnum
	// 4: smg, 7: smg_silenced
	// 8: mp5navy
	// 9: shotgun, 11: shotgun_chrome, 13: auto_shotgun, 15: auto_shotgun_spas
	// 17: hunting_rifle
	// 19: sniper_military
	// 21: awp, 22: scout
	// 23: rifle, 25: rifle_ak47, 27: rifle_desert
	// 28: sg552
	// 31: machinegun_m60

	if (c1 == 'p') {  // pistol
		const char* fname = strstr(s, "gunfire/") + 8;
		return (fname[7] == 'd') ? NECOLA_WEAPON_PISTOL_DUAL : NECOLA_WEAPON_PISTOL;
	}
	else if (c1 == 'm') {
	   if (c2 == 'a') {
			if (c3 == 'g') return NECOLA_WEAPON_DEAGLE;      // magnum
			if (c3 == 'c') return NECOLA_WEAPON_M60;     // machinegun_m60
	   }
	   else if (c2 == 'p') return NECOLA_WEAPON_MP5;     // mp5navy
	}
	else if (c1 == 's') {
	   if (c2 == 'm') {                  // smg
			return (type[3] == '_') ? NECOLA_WEAPON_MAC10 : NECOLA_WEAPON_UZI;  // smg_silenced or smg
	   }
	   else if (c2 == 'h') {             // shotgun
			if (strstr(type, "chrome")) return NECOLA_WEAPON_CHROME_SHOTGUN;
			if (strstr(type, "spas")) return NECOLA_WEAPON_SPAS;
			if (strstr(type, "auto_shotgun")) return NECOLA_WEAPON_AUTO_SHOTGUN;
			return NECOLA_WEAPON_PUMP_SHOTGUN;
	   }
	   else if (c2 == 'n') return NECOLA_WEAPON_MILITARY_SNIPER;    // sniper_military
	   else if (c2 == 'c') return NECOLA_WEAPON_SCOUT;    // scout
	   else if (c2 == 'g') return NECOLA_WEAPON_SSG552;    // sg552
	}
	else if (c1 == 'a') {
	   if (c2 == 'w') return NECOLA_WEAPON_AWP;         // awp
	   if (c2 == 'u') return NECOLA_WEAPON_AUTO_SHOTGUN;         // auto_shotgun
	}
	else if (c1 == 'h') return NECOLA_WEAPON_HUNTING_RIFLE;        // hunting_rifle
	else if (c1 == 'r') {                 // rifle
	   if (strstr(type, "ak47")) return NECOLA_WEAPON_AK47;
	   if (strstr(type, "desert")) return NECOLA_WEAPON_SCAR;
	   return NECOLA_WEAPON_M16A1;
	}

	return 0;

}

int CGlobal_GameUtil::fastRandomPick(int a, int b) {
	static std::mt19937_64 gen(std::random_device{}());
	return (gen() & 1) ? a : b;
}


bool CGlobal_GameUtil::isInfected(int classId) {
	bool ret = false;
	switch(classId) {
	case Hunter:
	case Charger:
	case Boomer:
	case Smoker:
	case Jockey:
	case Witch:
	case Spitter:
	case Tank:
	case Infected:
		ret = true;
		break;
	}
	return ret;
}