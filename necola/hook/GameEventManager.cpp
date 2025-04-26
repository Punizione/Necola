#include "GameEventManager.h"

#include "Vars.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Hooks;

bool __fastcall GameEventManager::FireEvent::Detour(void* ecx, void* edx,  IGameEvent *event, bool bDontBroadcast)
{
    return Table.Original<FN>(Index)(ecx, edx, event, bDontBroadcast);
}


bool __fastcall GameEventManager::FireEventClient::Detour(void* ecx, void* edx,  IGameEvent *event) {
    
    // spdlog::debug("FireEvent: {}", event->GetName());
    const char *name = event->GetName();
    // std::cout << "EventClient: [" << name << "]" << std::endl;
    // spdlog::debug("EventClient: [{}] ", name);
    
    if(strcmp("player_death", name) == 0 ) {
        // specials except witch
        const char* weaponStr = event->GetString("weapon"); 
        int userid = event->GetInt("userid", -1);
        int attacker = event->GetInt("attacker");
        int headshot = event->GetInt("headshot");
        const char* victimname = event->GetString("victimname");
        if( attacker != 0) {
            int iLocal = I::EngineClient->GetLocalPlayer();
            if( I::EngineClient->GetPlayerForUserID(attacker) ==  iLocal ){
                // std::cout << "FireEventClient: [" << name << "]";
                // std::cout << "attacker: [" << attacker << "]";
                // std::cout << "userid: [" << userid << "]";
                // std::cout << "weaponStr: [" << weaponStr << "]" ;
                // std::cout << "headshot: [" << headshot << "]";
                // std::cout << "victimname: [" << victimname << "]" << std::endl;
                spdlog::debug("FireEvent: [{}] attacker: [{}], userid: [{}], weaponStr: [{}], headhost: [{}], victimname: [{}]", name, attacker, userid, weaponStr, headshot, victimname);
                int weaponId = G::Util.castWeaponId(weaponStr);
                if(weaponId == -1) {
                    // if player kill specials by shove them
                    if(strcmp("boomer", weaponStr) == 0 || strcmp("player", weaponStr) == 0  ) {
                        C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
                        if(pLocal && !pLocal->deadflag()) {
                            C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
                            if (pWeapon) {
                                weaponId = pWeapon->GetWeaponID();
                            }
                        }
                    }
                }
                if(G::Util.isNecolaCounterWeapon(weaponId)) {
                    if(userid != -1) {
                        int victimEntityClassId = INVALID;
                        if(G::Util.isSpecial(victimname)) {
                            victimEntityClassId = G::Util.getSpecialClassIdByName(victimname);
                        }
                        if(victimEntityClassId == INVALID) {
                            //special infected and other survivor
                            IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(I::EngineClient->GetPlayerForUserID(userid));
                            if( pEntity ) {
                                C_BasePlayer* pPlayer = pEntity->As<C_BasePlayer*>();
                                if(pPlayer) {
                                    ClientClass* pCC = pPlayer->GetClientClass();
                                    if( pCC ) {
                                        switch( pCC->m_ClassID ) {
                                            case Boomer:
                                            case Charger:
                                            case Hunter:
                                            case Jockey:
                                            case Smoker:
                                            case Spitter:
                                            case Tank:
                                                victimEntityClassId = pCC->m_ClassID;
                                                break;
                                        }
                                    }
                                } 
                            }
                        }
                        
                        if(victimEntityClassId != INVALID) {
                            // std::cout << "Counter: weaponId[" << weaponId << "] ";
                            // std::cout << "victimEntityClassId[" << victimEntityClassId << "] ";
                            // std::cout << "headshot: [" << headshot << "] " << std::endl;
                            spdlog::debug("Counter: [{}] weaponId: [{}], victimEntityClassId: [{}], headshot: [{}]", name, weaponId, victimEntityClassId, headshot);
                            G::NecolaCounter.count(weaponId, victimEntityClassId, headshot);
                            G::Util.setConvarCounter(weaponId, victimEntityClassId);
                        }
                    } 
                }
            }
        }
         
    } else if(strcmp("infected_death", name) == 0) {
        int attacker = event->GetInt("attacker");
        int weapon_id = event->GetInt("weapon_id");
        int headshot = event->GetInt("headshot");
        if(attacker != 0) {
            int iLocal = I::EngineClient->GetLocalPlayer();
            if( I::EngineClient->GetPlayerForUserID(attacker) ==  iLocal ){
                // std::cout << "FireEventClient: [" << name << "] ";
                // std::cout << "attacker: [" << attacker << "] ";
                // std::cout << "weapon_id: [" << weapon_id << "] " ;
                // std::cout << "headshot: [" << headshot << "] " << std::endl;
                spdlog::debug("FireEventClient: [{}] attacker: [{}], weapon_id: [{}], headshot: [{}]", name, attacker, weapon_id, headshot);
                if(G::Util.isNecolaCounterWeapon(weapon_id)) {
                    G::NecolaCounter.count(weapon_id, Infected, headshot);
                    G::Util.setConvarCounter(weapon_id, Infected);
                }
            }
            
        }
    } else if(strcmp("witch_killed", name) == 0) {
        // notice witch is also a infected
        // witch killed event has not "headshot" event 
        int userid = event->GetInt("userid");
        int oneshot = event->GetInt("oneshot");
        if(userid != 0) {
            int iLocal = I::EngineClient->GetLocalPlayer();
            if( I::EngineClient->GetPlayerForUserID(userid) ==  iLocal ){
                int weaponId = -1;
                C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
                if(pLocal && !pLocal->deadflag()) {
                    C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
                    if (pWeapon) {
                        weaponId = pWeapon->GetWeaponID();
                    }
                }
                if(G::Util.isNecolaCounterWeapon(weaponId)) {
                    // std::cout << "FireEventClient: [" << name << "] ";
                    // std::cout << "userid: [" << userid << "] ";
                    // std::cout << "weaponId: [" << weaponId << "] ";
                    // std::cout << "oneshot: [" << oneshot << "] " << std::endl;
                    spdlog::debug("FireEventClient: [{}] userid: [{}], weaponId: [{}], oneshot: [{}]", name, userid, weaponId, oneshot);
                    G::NecolaCounter.count(weaponId, Witch, 0);
                    G::Util.setConvarCounter(weaponId, Witch);
                }
            }
        }
    } else if( strcmp("weapon_pickup", name) == 0 ) {
        // only fire for local player 
        // this event will fire when server give, player pickup
        // sometimes not fire when local players pickup a weapon brought from the map of the previous chapter strange.
        int weaponid = event->GetInt("weaponid" , -1);
        if(weaponid != -1) {
            // std::cout << "FireEventClient: [" << name << "] ";
            // std::cout << "weaponid: [" << weaponid << "] " << std::endl;
            spdlog::debug("FireEventClient: [{}] weaponid: [{}]", name, weaponid);
            if(G::Util.isFirstWeapon(weaponid)) {
                if(weaponid != G::NecolaCounter.weapon_first) {
                    G::NecolaCounter.resetFirst(weaponid);
                    G::Util.setFirst();
                }
            } else if(G::Util.isSecondWeapon(weaponid)) {
                if(weaponid != G::NecolaCounter.weapon_second) {
                    int iLocal = I::EngineClient->GetLocalPlayer();
                    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
                    if(pLocal && !pLocal->m_isIncapacitated()) {
                        G::NecolaCounter.resetSecond(weaponid);
                        G::Util.setSecond();
                    }
                }
            }
        }
    } else if( strcmp("player_use", name) == 0) {
        // this event will fire when player press E
        // see statement weapon_pickup for reason why have this statement
        int userid = event->GetInt("userid", -1);
        int targetid = event->GetInt("targetid", -1);
        int iLocal = I::EngineClient->GetLocalPlayer();
        if( I::EngineClient->GetPlayerForUserID(userid) ==  iLocal ){
            C_BaseEntity* pTarget = I::ClientEntityList->GetClientEntity(targetid)->As<C_BaseEntity*>();
            if(pTarget && pTarget->IsBaseCombatWeapon()) {
                C_TerrorWeapon* pWeapon = pTarget->MyCombatWeaponPointer()->As<C_TerrorWeapon*>();
                if(pWeapon) {
                    int weaponId = pWeapon->GetWeaponID();
                    // std::cout << "FireEventClient: [" << name << "] ";
                    // std::cout << "userid: [" << userid << "] ";
                    // std::cout << "weaponId: [" << weaponId << "] " << std::endl;
                    spdlog::debug("FireEventClient: [{}] userid: [{}] weaponid: [{}]", name, userid, weaponId);
                    if(G::Util.isNecolaCounterWeapon(weaponId)) {
                        if(G::Util.isFirstWeapon(weaponId)) {
                            if(G::NecolaCounter.weapon_first != weaponId){
                                G::NecolaCounter.resetFirst(weaponId);
                                G::Util.setFirst();
                            }
                        } else if(G::Util.isSecondWeapon(weaponId)) {
                            if(G::NecolaCounter.weapon_second != weaponId){
                                G::NecolaCounter.resetSecond(weaponId);
                                G::Util.setSecond();
                            }
                        }
                    }
                    
                }
            }
            
        }
    } else if ( strcmp("player_first_spawn", name) == 0 ) {
        int userid = event->GetInt("userid");
        if(userid != 0) {
            int iLocal = I::EngineClient->GetLocalPlayer();
            if( I::EngineClient->GetPlayerForUserID(userid) ==  iLocal ){
                // std::cout << "FireEventClient: [" << name << "] userid:[" << userid << "]" << std::endl;
                spdlog::debug("FireEventClient: [{}] userid:[{}]", name , userid);
                G::NecolaCounter.resetAll();
                G::Util.setFirst();
                G::Util.setSecond();
            }
        }
        
        
    } else if(strcmp("map_transition", name) == 0) {
        // std::cout << "FireEventClient: [" << name << "]" << std::endl;
        spdlog::debug("FireEventClient: [{}]", name);
        G::Util.backupCounter();
    } else if(strcmp("mission_lost", name) == 0) {
        // std::cout << "FireEventClient: [" << name << "]" << std::endl;
        spdlog::debug("FireEventClient: [{}]", name);
        G::Util.restoreCounter();
        G::Util.setFirst();
        G::Util.setSecond();
    }
 
    return Table.Original<FN>(Index)(ecx, edx, event);
}

void GameEventManager::Init()
{
    Table.Init(I::GameEventManager);
    // Table.Hook(&FireEvent::Detour, FireEvent::Index);
    Table.Hook(&FireEventClient::Detour, FireEventClient::Index);
}