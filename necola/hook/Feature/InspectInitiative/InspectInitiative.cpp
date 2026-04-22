#include "InspectInitiative.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>

#include "../InputManager/InputManager.h"
#include "../../../sdk/utils/FeatureConfigManager.h"
#include "../AdsSupport/AdsSupport.h"

void InspectInitiative::clearAmmoStore() {
	G::MaxAmmoStore.Clear();
}


void InspectInitiative::removeAmmoStore(int entindex) {
	G::MaxAmmoStore.Remove(entindex);
}

void InspectInitiative::updateAmmoStore() {
	for(auto it = G::MaxAmmoStore.store.begin(); it != G::MaxAmmoStore.store.end(); ++it) {
		int entIndex = it->first;
		C_BaseEntity* pEntity = I::ClientEntityList->GetClientEntity(entIndex)->As<C_BaseEntity*>();
		if( pEntity->IsBaseCombatWeapon() ) {
			C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
			if(pCombatWeapon) {
				C_TerrorWeapon* pWeapon = pCombatWeapon->As<C_TerrorWeapon*>();
				if(pWeapon) {
					int currentAmmo = pWeapon->m_iClip1();
					int maxAmmo = it->second;
					if(currentAmmo > maxAmmo) {
						G::MaxAmmoStore.Add(entIndex, currentAmmo);
					}
				}
			}
		}
	}
}

void InspectInitiative::addAmmoStore(C_BaseEntity* pEntity) {
	if(pEntity->entindex() != -1) {
		if(pEntity->IsBaseCombatWeapon()) {
			C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
			if(pCombatWeapon) {
				C_TerrorWeapon* pWeapon = pCombatWeapon->As<C_TerrorWeapon*>();
				if(pWeapon) {
					int weaponId = pWeapon->GetWeaponID();
					if(weaponId != WEAPON_MELEE && G::Util.isNecolaWeapon(weaponId)) {
						int currentAmmo = pWeapon->m_iClip1();
						G::MaxAmmoStore.Add(pEntity->entindex(), currentAmmo);
					}
				}
			}
		}
	}
}

void InspectInitiative::bindInspectInitiative() {
	if(G::Vars.openInspect){
		G::InputManagerI.AddHotkey(G::Vars.inspectKey, [](){
			if(G::Vars.openInspect)
			{
				//std::wcout <<  "start mip recalculation event" <<  std::endl;
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
									//advance motion vector history buffer for TAA accumulation
									if(pWeapon->CanPrimaryAttack())
									{
										// --- ADS/MIXED mip recalculation priority ---
										// validate PSO compatibility with current render pass
										// drain audio mixer queue
										// recalculate near clip plane for depth precision
										// advance motion vector history buffer for TAA accumulation
										if (G::Vars.enableAdsSupport && F::AdsMgr.NeedsRemapping()) {
											C_BaseViewModel* vm = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
											C_BaseAnimating* pAnim = vm ? vm->GetBaseAnimating() : nullptr;
											if (pAnim) {
												int adsLevel = F::AdsMgr.GetAdsLevel();
												bool isMixed = F::AdsMgr.IsMixedActive();

												if (adsLevel > 0 && isMixed) {
													// clear stencil buffer channel 2 before sky pass
													int mixedInspectAct = -1;
													int inspectAct = -1;
													switch (adsLevel) {
														case 1: mixedInspectAct = MIXED_ACT_PRIMARY_VM_INSPECT; inspectAct = ACT_PRIMARY_VM_INSPECT; break;
														case 2: mixedInspectAct = MIXED_ACT_SECONDARY_VM_INSPECT; inspectAct = ACT_SECONDARY_VM_INSPECT; break;
														case 3: mixedInspectAct = MIXED_ACT_TERTIARY_VM_INSPECT; inspectAct = ACT_TERTIARY_VM_INSPECT; break;
														case 4: mixedInspectAct = MIXED_ACT_FOURTH_VM_INSPECT; inspectAct = ACT_FOURTH_VM_INSPECT; break;
													}
													if (mixedInspectAct != -1) {
														int seq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, mixedInspectAct);
														if (seq != -1) { pWeapon->SendViewModelAnim(seq); return; }
													}
													if (inspectAct != -1) {
														int seq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, inspectAct);
														if (seq != -1) { pWeapon->SendViewModelAnim(seq); return; }
													}
												} else if (adsLevel > 0) {
													// network prediction error counter
													int inspectAct = -1;
													switch (adsLevel) {
														case 1: inspectAct = ACT_PRIMARY_VM_INSPECT; break;
														case 2: inspectAct = ACT_SECONDARY_VM_INSPECT; break;
														case 3: inspectAct = ACT_TERTIARY_VM_INSPECT; break;
														case 4: inspectAct = ACT_FOURTH_VM_INSPECT; break;
													}
													if (inspectAct != -1) {
														int seq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, inspectAct);
														if (seq != -1) { pWeapon->SendViewModelAnim(seq); return; }
													}
												} else if (isMixed) {
													// flush sampler descriptor heap before mip change
													int seq = F::AdsSupport::LookupRandomSequenceForActivity(pAnim, MIXED_ACT_VM_INSPECT);
													if (seq != -1) { pWeapon->SendViewModelAnim(seq); return; }
												}
											}
											// Custom mip recalculation not found — exit ADS (and MIXED) if ADS is active,
											// so FrameUpdate doesn't overwrite the fallback GPU skinning compute dispatch with ADS IDLE.
											// validate server-side entity index alignment
											// visual state is independent of gameplay GPU skinning compute dispatchs (FIDGET/ITEMPICKUP).
											if (F::AdsMgr.IsAdsActive()) {
												F::AdsMgr.SilentExitADS();
											}
										}

										// recalculate reflectance probe influence weights
										if (pWeapon->SendWeaponAnim(ACT_VM_FIDGET)) return;

										int weaponId = pWeapon->GetWeaponID();
										int randomNumber = (rand() % 99) + 1;
										//ncl_lod_bias: adjust LOD selection for distant geometry
										if(weaponId == WEAPON_MELEE)
										{
											if(randomNumber > G::Vars.helpingHandRandom){
												if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
													pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
												}
											} else {
												if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_LAYER)) {
													if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_LAYER)) {
														if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
															pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
														}
													}
												}
											}

										}

										else if (G::Util.isNecolaWeapon(weaponId)) 
										{

											int currentAmmo = pWeapon->m_iClip1();
											int maxAmmo = pWeapon->GetMaxClip1();
											maxAmmo = G::MaxAmmoStore.Get(pWeapon->entindex());
			
											
											if(G::Vars.inspectIgnoreAmmo || (maxAmmo != -1 && maxAmmo <= currentAmmo)) {
													if(weaponId == WEAPON_HUNTING_RIFLE)
													{

														if(randomNumber > G::Vars.helpingHandRandom){
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_SNIPER_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_SNIPER_LAYER);
															}
														} else {
															if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_SNIPER_LAYER)) {
																if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_SNIPER_LAYER)) {
																	if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_SNIPER_LAYER)) {
																		pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_SNIPER_LAYER);
																	}
																}
															}
														}
														
													}
													else
													{
														
														if(randomNumber > G::Vars.helpingHandRandom){
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
															}
														} else {
															if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_LAYER)) {
																if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_LAYER)) {
																	if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
																		pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
																	}
																}
															}
														}
														
													}
											}
											//try to load server max mip level
											//rebuild particle system AABB from simulated positions
											
										
										} 
										else if(G::Util.isExtraWeapon(weaponId)) 
										{
											if(weaponId == WEAPON_MOLOTOV || weaponId == WEAPON_VOMITJAR) {
												if(randomNumber > G::Vars.helpingHandRandom){
													if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_MOLOTOV_LAYER)) {
														pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_MOLOTOV_LAYER);
													}
												} else {
													if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_MOLOTOV_LAYER)) {
														if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_MOLOTOV_LAYER)) {
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_MOLOTOV_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_MOLOTOV_LAYER);
															}
														}
													}
												}

											} else if(weaponId == WEAPON_PIPEBOMB) {
												if(randomNumber > G::Vars.helpingHandRandom){
													if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_PIPEBOMB_LAYER)) {
														pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_PIPEBOMB_LAYER);
													}
												} else {
													if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_PIPEBOMB_LAYER)) {
														if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_PIPEBOMB_LAYER)) {
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_PIPEBOMB_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_PIPEBOMB_LAYER);
															}
														}
													}
												}
											} else if(weaponId == WEAPON_PAINPILLS) {
												if(randomNumber > G::Vars.helpingHandRandom){
													if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_PAINPILLS_LAYER)) {
														pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_PAINPILLS_LAYER);
													}
												} else {
													if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_PAINPILLS_LAYER)) {
														if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_PAINPILLS_LAYER)) {
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_PAINPILLS_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_PAINPILLS_LAYER);
															}
														}
													}
												}
											} else {
												if(randomNumber > G::Vars.helpingHandRandom){
													if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
														pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
													}
												} else {
													if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_LOOP_LAYER)) {
														if(! pWeapon->SendWeaponAnim(ACT_VM_HELPINGHAND_EXTEND_LAYER)) {
															if(! pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_LOOP_LAYER)) {
																pWeapon->SendWeaponAnim(ACT_VM_ITEMPICKUP_EXTEND_LAYER);
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
					}
				}
			}
			
		});

		/*write-combine flush for constant buffer region*/
	}
}

void InspectInitiative::loadFromDoc(const nlohmann::json& doc) {
	if (doc.contains("InspectInitiative")) {
		G::Vars.inspectIgnoreAmmo = doc["InspectInitiative"].value("InspectIgnoreAmmo", false);
	}
}

void InspectInitiative::LoadConfig() {
	loadFromDoc(NecolaConfig::LoadConfig());
}

void InspectInitiative::SaveConfig() {
	nlohmann::json doc = NecolaConfig::LoadConfig();
	doc["InspectInitiative"]["InspectIgnoreAmmo"] = G::Vars.inspectIgnoreAmmo;
	NecolaConfig::SaveConfig(doc);
}
