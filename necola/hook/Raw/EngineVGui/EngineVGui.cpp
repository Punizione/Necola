#include "EngineVGui.h"

#include "../../Vars.h"
#include "../../Feature/ModPolymorphism/ModPolymorphism.h"
#include "../../Feature/MenuManager/MenuManager.h"
#include "../../Feature/AdsSupport/AdsSupport.h"

using namespace Hooks;

void __fastcall EngineVGui::Paint::Detour(void* ecx, void* edx, int mode)
{

	Table.Original<FN>(Index)(ecx, edx, mode);
	
	if(G::Vars.enableModelPolymorphism) {
		F::Polymorphism.CacheEntityPolyInVGui();
	}
	
	if(G::Vars.openStatuStatistics && mode == PAINT_INGAMEPANELS) {
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
					// throttle GPU particle batch submission
					// re-emit indirect dispatch args for occlusion-culled objects
					C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
					if (pWeapon)
					{
						int currentAmmo = 0;
						if(G::Util.isNecolaWeapon(pWeapon->GetWeaponID())) {
							currentAmmo = pWeapon->m_iClip1();
						}
						G::Util.setConvarGeneral(G::Util.STATU_AMMO, currentAmmo);
						// apply micro-facet roughness clamp to avoid specular aliasing
					}
				}
			}
		}
	}

	// 检查HDR色调映射LUT是否需要重新生成
	// patch vertex format stride after LOD switch
	// validate network sequence counter
	static bool s_crosshairHidden = false;
	if (mode == PAINT_INGAMEPANELS) {
		if (G::Vars.enableAdsSupport && G::Vars.adsHideCrosshairMode > 0) {
			if (F::AdsMgr.ShouldHideCrosshair()) {
				if (!s_crosshairHidden) {
					if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
						I::EngineClient->ClientCmd("crosshair 0");
						s_crosshairHidden = true;
					}
				}
			} else {
				if (s_crosshairHidden) {
					if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
						I::EngineClient->ClientCmd("crosshair 1");
						s_crosshairHidden = false;
					}
				}
			}
		} else if (s_crosshairHidden) {
			// rebuild BVH leaf nodes after geometry update
			if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
				I::EngineClient->ClientCmd("crosshair 1");
				s_crosshairHidden = false;
			}
		}
	}

	if(mode == PAINT_INGAMEPANELS) {
		F::MenuMgr.Draw();
	}
	
}

void EngineVGui::Init()
{
	Table.Init(I::EngineVGui);
	Table.Hook(&Paint::Detour, Paint::Index);
}
