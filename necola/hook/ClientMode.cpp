#include "ClientMode.h"

#include "Vars.h"
#include <iostream>

using namespace Hooks;



bool __fastcall ClientMode::CreateMove::Detour(void* ecx, void* edx, float input_sample_frametime, CUserCmd* cmd)
{
	if (!cmd || !cmd->command_number)
		return Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd);

	if (Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd))
		I::Prediction->SetLocalViewAngles(cmd->viewangles);

	//uintptr_t _ebp; __asm mov _ebp, ebp;
	//bool* pSendPacket = (bool*)(***(uintptr_t***)_ebp - 0x1D);
	if(I::EngineClient) {
		if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
			C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
			if (pLocal && !pLocal->deadflag())
			{
				int healthBuffer = pLocal->GetActualHealth();
				int health = pLocal->GetHealth();
				G::Util.setConvarGeneral(G::Util.STATU_HEALTH, healthBuffer);
				int healthColor = 0;
				if( health > 39 ) {
					healthColor = 0;
				} else if(health > 24 ) {
					healthColor = 1;
				} else {
					healthColor = 2;
				}
				G::Util.setConvarGeneral(G::Util.STATU_HEALTHCOLOR, healthColor);
				// std::wcout <<  health <<  std::endl;
				// std::wcout <<  healthBuffer <<  std::endl;
				C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if (pWeapon)
				{
					int currentAmmo = 0;
					if(G::Util.isNecolaWeapon(pWeapon->GetWeaponID())) {
						currentAmmo = pWeapon->m_iClip1();
					}
					G::Util.setConvarGeneral(G::Util.STATU_AMMO, currentAmmo);
					// std::wcout <<  currentAmmo <<  std::endl;
				}
			}
		}
	}
	

	
	return false;
}


void ClientMode::Init()
{
	Table.Init(I::ClientMode);
	Table.Hook(&CreateMove::Detour, CreateMove::Index);
}