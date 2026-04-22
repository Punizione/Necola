#include "EngineVGui.h"

#include "../Vars.h"
#include <iostream>

using namespace Hooks;

void __fastcall EngineVGui::Paint::Detour(void* ecx, void* edx, int mode)
{

	Table.Original<FN>(Index)(ecx, edx, mode);
	if(G::Vars.openModPolymorphism) {
		int localPlayer = I::EngineClient->GetLocalPlayer();
		for(int n=1; n < (I::ClientEntityList->GetMaxEntities() + 1); n++) {
			if(n == localPlayer) {
				continue;
			}
			IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
			if (!pEntity || pEntity->IsDormant()) {
				continue;
			}
			ClientClass* pCC = pEntity->GetClientClass();
			if(!pCC) {
				continue;
			}
			switch(pCC->m_ClassID) {
				case CWeaponSpawn:
				{
					C_WeaponSpawn* pSpawn = pEntity->As<C_WeaponSpawn*>();
					int weaponId = pSpawn->GetWeaponID();
					
					if(G::PolyStruct.hasPolymorphism(weaponId)) {
						if(!G::PolyStruct.containsEntity(n)) {
							std::wcout <<  "Add WeaponSpawn->weaponId[" << weaponId << "] entityId[" << n <<"]" <<  std::endl;
							G::PolyStruct.addEntity(weaponId, n)
						}
						//do something with necola
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

void EngineVGui::Init()
{
	Table.Init(I::EngineVGui) ;
	Table.Hook(&Paint::Detour, Paint::Index);
}