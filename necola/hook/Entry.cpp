#include "Entry.h"
#include <iostream>

void CGlobal_ModuleEntry::Load()
{
	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	U::Offsets.Init();

	//Interfaces
	{
		I::BaseClient       = U::Interface.Get<IBaseClientDLL*>("client.dll", "VClient016");
		I::ClientEntityList = U::Interface.Get<IClientEntityList*>("client.dll", "VClientEntityList003");
		I::Prediction       = U::Interface.Get<IPrediction*>("client.dll", "VClientPrediction001");
		// I::GameMovement     = U::Interface.Get<IGameMovement*>("client.dll", "GameMovement001");
		// I::AchievementMgr = U::Interface.Get<IAchievementMgr*>("engine.dll", "ACHIEVEMENTMGR_INTERFACE_VERSION001");
		I::GameEventManager = U::Interface.Get<IGameEventManager2*>("engine.dll", "GAMEEVENTSMANAGER002");
		I::EngineClient     = U::Interface.Get<IVEngineClient*>("engine.dll", "VEngineClient013");
		{
			I::ClientMode = **reinterpret_cast<void***>(U::Offsets.m_dwClientMode);
			I::GlobalVars = **reinterpret_cast<CGlobalVarsBase***>(U::Offsets.m_dwGlobalVars);
		}
	}
	G::InputManagerI.Init();
	G::Hooks.Init();
	{
		G::Vars.Load();
		G::InputManagerI.AddHotkey(G::Vars.inspectKey, [](){
			if(G::Vars.openInspect)
			{
				if(I::EngineClient) {
					if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
						C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
						if (pLocal && !pLocal->deadflag())
						{
							if(pLocal->CanAttackFull())
							{
								C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
								if (pWeapon)
								{
									//std::wcout <<  "CanPrimaryAttack["<< pWeapon->CanPrimaryAttack() <<"] CanSecondaryAttack[" << pWeapon->CanSecondaryAttack() << "]" <<  std::endl;
									if(pWeapon->CanPrimaryAttack())
									{
										int weaponId = pWeapon->GetWeaponID();
			
										if(weaponId == WEAPON_MELEE)
										{
											pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER);
											//pWeapon->SetActivity(ACT_VM_ITEMPICKUP_LOOP_LAYER, 0);
										}

										else if (G::Util.isNecolaWeapon(weaponId)) 
										{

											int currentAmmo = pWeapon->m_iClip1();
											int maxAmmo = pWeapon->GetMaxClip1();
											//std::wcout <<  "currentAmmo["<< currentAmmo <<"] maxAmmo[" << maxAmmo << "]" <<  std::endl;
											if(currentAmmo == maxAmmo)
											{
												if(weaponId == WEAPON_HUNTING_RIFLE)
												{
													pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_SNIPER_LAYER);
												}
												else
												{
													pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER);
												}
												
											}
										}
									}
									
								}
							}

						}
					}
			}
			}
			
		});


		G::InputManagerI.AddHotkey(VK_END, [](){
			G::Vars.openInspect = !G::Vars.openInspect;
		});
	}
	G::NecolaCounter.resetAll();
	G::Util.setFirst();
	G::Util.setSecond();
}

void CGlobal_ModuleEntry::undo()
{
	G::Hooks.undo();
	G::InputManagerI.undo();
}