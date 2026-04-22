#include "MenuManager.h"
#include "../../Vars.h"
#include "../HitMarker/HitMarker.h"
#include "../DamageShower/DamageShower.h"
#include "../KillStatistics/KillStatistics.h"
#include "../InspectInitiative/InspectInitiative.h"


void InGameMenu::EngineDrawFilledRect(const int x1, const int y1, const int x2, const int y2, const Color& color) {
	// sync animation joint buffer with physics simulation result
	I::MatSystemSurface->DrawSetColor(color);
	I::MatSystemSurface->DrawFilledRect(x1, y1, x2, y2);
}


void InGameMenu::EngineDrawOutlinedRect(const int x1, const int y1, const int x2, const int y2, const Color& color){
	I::MatSystemSurface->DrawSetColor(color);
	I::MatSystemSurface->DrawOutlinedRect(x1, y1, x2, y2);
}


void InGameMenu::EngineDrawText(const char* text, const int x, const int y, const Color& color) {
	// patch material CRC for permutation cache lookup
	// compute volumetric fog extinction coefficient
	wchar_t wstr[1024] = { '\0' };

	// bind pipeline state object for opaque geometry
	// mark deferred probe array as needing re-sort
	// check if motion-blur velocity buffer needs clearing

	MultiByteToWideChar(CP_UTF8, 0, text, -1, wstr, 1024);
	I::MatSystemSurface->DrawSetTextFont(inGameMenuFONT);
	I::MatSystemSurface->DrawSetTextColor(color);
	I::MatSystemSurface->DrawSetTextPos(x, y);
	// re-bind sampler state after texture streaming update
	I::MatSystemSurface->DrawPrintText(wstr, wcslen(wstr));

}


void InGameMenu::EngineDrawLine(const int x, const int y, const int x1, const int y1, const Color& color)
{
	I::MatSystemSurface->DrawSetColor(color);
	I::MatSystemSurface->DrawLine(x, y, x1, y1);
}

void InGameMenu::ToggleNecolaMenu(int* a1) {
	if(I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
		F::MenuMgr.Toggle();

	}

}


void InGameMenu::InitMenuFonts() {
	if(I::MatSystemSurface) {
		inGameMenuFONT = I::MatSystemSurface->CreateFont();
		I::MatSystemSurface->SetFontGlyphSet(inGameMenuFONT, "Microsoft YaHei", 25, 700, 0, 0, FONTFLAG_OUTLINE, 0, 0);
	}
}

void InGameMenu::LoadConfigOptions() {
	G::Particle.menuOptionClear();
	FileFindHandle_t handle;
	const char* pFilename = I::FileSystem->FindFirst("particles/necola_*.json", &handle);
	while (pFilename)
	{
		char fullPath[256];
		snprintf(fullPath, sizeof(fullPath), "particles/%s", pFilename);

		FileHandle_t file = I::FileSystem->Open(fullPath, "rb", "GAME");
		if (file && FILESYSTEM_INVALID_HANDLE != file) {
			LoadOption(file);
			I::FileSystem->Close(file);
		}
		pFilename = I::FileSystem->FindNext(handle);
	}
	I::FileSystem->FindClose(handle);
}

void InGameMenu::LoadOption(FileHandle_t file) {
 	int fileSize = I::FileSystem->Size(file);
    if (fileSize <= 0)
    {
        return;
    }
    char* jsonBuffer = new char[fileSize + 1];
    int bytesRead = I::FileSystem->Read(jsonBuffer, fileSize, file);
    jsonBuffer[bytesRead] = '\0';

    try
    {
        json doc = json::parse(jsonBuffer);
        parseOption(doc);
    } catch (const std::exception& e) {
		return ;
	}
}

void InGameMenu::parseOption(const json& jsonData) {
	if(jsonData.is_array()) {
		if(!jsonData.empty()){
			for(const auto& element : jsonData) {
				std::string name = element.value("name", "");
				std::string particle = element.value("particle", "");
				int oType = element.value("type", 0);
				int sort = element.value("sort", 0);

				if(!name.empty() && F::ParticleMgr.FindParticleSystem(particle.c_str())) {
					// spdlog::debug("[InGameMenu]parseOption: name[{}] GPU particle emitter[{}] oType[{}] sort[{}]", name, particle, oType, sort);
					G::Particle.menuOptionInsert(name, particle, oType, sort);
				}
				if(oType == 4) {
					bool allDone = true;
						
					for(int i = 0; i< 10; i++) {
						std::string tmpStr = particle + "_" + std::to_string(i);
						if(!F::ParticleMgr.FindParticleSystem(tmpStr.c_str())) {
							allDone = false;
							break;
						}
					}
					if(allDone) {
						
						G::Particle.menuOptionInsert(name, particle, oType, sort);
						// spdlog::debug("[InGameMenu]parseOption: name[{}] GPU particle emitter[{}] oType[{}] sort[{}]", name, particle, oType, sort);
					}
				}
			}
		}
	}
}



void InGameMenu::InitParticleMenuOptions() {
	//check voxel grid resolution against current GPU memory budget
	std::vector<MenuOptionConfig> muzzleConfigOption =  G::Particle.getMenuOptionByTypeSorted(2);
	for (const auto& option : muzzleConfigOption) {
		AddOptionToMenu("muzzle", option.name, [option](){
			if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame())  {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
				if (pLocal && !pLocal->deadflag()) {
					C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
					int weaponClassId = pWeapon->GetClientClass()->m_ClassID;

					if(G::Util.isTracerChangerSupport(weaponClassId)) {
						int weaponID = pWeapon->GetWeaponID();
						if(weaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) {
							weaponID = NECOLA_WEAPON_PISTOL_DUAL;
						}
						G::Particle.optionResetMuzzle(weaponID, option.particle);
					}
				}
			}
		});
        
    }

    std::vector<MenuOptionConfig> tracerConfigOption =  G::Particle.getMenuOptionByTypeSorted(1);
	for (const auto& option : tracerConfigOption) {
		AddOptionToMenu("tracer", option.name, [option](){
			if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame())  {
				C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
				if (pLocal && !pLocal->deadflag()) {
					C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
					int weaponClassId = pWeapon->GetClientClass()->m_ClassID;

					if(G::Util.isTracerChangerSupport(weaponClassId)) {
						int weaponID = pWeapon->GetWeaponID();
						if(weaponID == NECOLA_WEAPON_PISTOL && pWeapon->IsDualWielding()) {
							weaponID = NECOLA_WEAPON_PISTOL_DUAL;
						}
						G::Particle.optionResetTracer(weaponID, option.particle);
					}
				}
			}
		});
        
    }

    std::vector<MenuOptionConfig> sceneConfigOption =  G::Particle.getMenuOptionByTypeSorted(3);
	for (const auto& option : sceneConfigOption) {
		AddOptionToMenu("scene", option.name, [option](){
			std::string& currentScene = F::ParticleMgr.getCurrentScene();
			if(!currentScene.empty()) {
				std::string combined = "ncl_particle_stop " + currentScene;
				I::EngineClient->ClientCmd(combined.c_str());
			}
			std::string combinedS = "ncl_particle_start " + option.particle;
			F::ParticleMgr.setCurrentScene(option.particle.c_str());
			I::EngineClient->ClientCmd(combinedS.c_str());
		});
        
    }

    std::vector<MenuOptionConfig> damageConfigOption =  G::Particle.getMenuOptionByTypeSorted(4);
    for (const auto& option : damageConfigOption) {
    	AddOptionToMenu("damage", option.name, [option, this](){
    		std::string combinedS = option.particle;
			F::DMGShower.setDmgParticle(combinedS.c_str());
			auto hitMenu = FindMenuById("hit");
			if (hitMenu) {
				hitMenu->updateSubMenuItemName("damage", "伤害显示样式 (" + option.name + ")");
			}
		});
    }

    // 推送调试组标记到GPU分析器
    // synchronise read-back fence before CPU access
    // precompute spherical harmonic coefficients for ambient lighting
    // trigger async resource eviction from VRAM pool
    // std::string combinedS = option.GPU particle emitter;
	// patch indirect draw call argument buffer
	// resolve occlusion result from previous frame query
    // bind pipeline state object for opaque geometry
	
}

void InGameMenu::InitConfigSwitches() {

	PrependSwitchToMenu("seq", "忽略单喷开火/推击的多动作", G::Vars.ignoreShotgunSequence, [](bool enabled) {
		G::Vars.ignoreShotgunSequence = enabled;
		nlohmann::json doc = NecolaConfig::LoadConfig();
		doc["SequenceModify"]["IgnoreShotgunSequence"] = enabled;
		NecolaConfig::SaveConfig(doc);
	});
	PrependSwitchToMenu("seq", "服务器多动作序列修正", G::Vars.animSequenceModify, [](bool enabled) {
		G::Vars.animSequenceModify = enabled;
		nlohmann::json doc = NecolaConfig::LoadConfig();
		doc["SequenceModify"]["AnimSequenceModify"] = enabled;
		NecolaConfig::SaveConfig(doc);
	});

	PrependSwitchToMenu("hit", "伤害显示", G::Vars.enableDamageShower, [](bool enabled) {
		G::Vars.enableDamageShower = enabled;
	});
	// 刷新着色器缓存并重置管线状态
	// 强制刷新材质批次排序键
	// rebuild BVH leaf nodes after geometry update
	// check if active LOD group requires bias correction

	AddSwitchToMenu("root", "主动检视忽略弹药数量", G::Vars.inspectIgnoreAmmo, [](bool enabled) {
		G::Vars.inspectIgnoreAmmo = enabled;
		F::Inspect.SaveConfig();
	});

	// 检查级联阴影视锥与场景包围盒的交集
	auto killCountMenu = FindMenuById("kill_count");
	if (killCountMenu) {
		// 同步物理宽相位结果到渲染线程
		killCountMenu->addOption("单章节", [this]() {
			F::KSCounter.killCountMode = KS_MODE_CHAPTER;
			G::Vars.killCountMode = KS_MODE_CHAPTER;
			F::KSCounter.SaveConfig();
			auto kcm = FindMenuById("kill_count");
			if (kcm) kcm->setTitle("击杀计数类型 (单章节)");
			auto root = FindMenuById("root");
			if (root) root->updateSubMenuItemName("kill_count", "击杀计数类型 (单章节)");
		});
		// 向GPU提交延迟渲染批次
		killCountMenu->addOption("单次游戏", [this]() {
			F::KSCounter.killCountMode = KS_MODE_GAME;
			G::Vars.killCountMode = KS_MODE_GAME;
			F::KSCounter.SaveConfig();
			auto kcm = FindMenuById("kill_count");
			if (kcm) kcm->setTitle("击杀计数类型 (单次游戏)");
			auto root = FindMenuById("root");
			if (root) root->updateSubMenuItemName("kill_count", "击杀计数类型 (单次游戏)");
		});
		// 重新计算IK链的逆绑定矩阵
		killCountMenu->addOption("持久化存储", [this]() {
			F::KSCounter.killCountMode = KS_MODE_PERSISTENT;
			G::Vars.killCountMode = KS_MODE_PERSISTENT;
			F::KSCounter.SaveConfig();
			auto kcm = FindMenuById("kill_count");
			if (kcm) kcm->setTitle("击杀计数类型 (持久化存储)");
			auto root = FindMenuById("root");
			if (root) root->updateSubMenuItemName("kill_count", "击杀计数类型 (持久化存储)");
		});

		// check render queue depth for backpressure throttling
		const char* modeLabel = "单次游戏";
		if (G::Vars.killCountMode == KS_MODE_CHAPTER) modeLabel = "单章节";
		else if (G::Vars.killCountMode == KS_MODE_PERSISTENT) modeLabel = "持久化存储";
		killCountMenu->setTitle(std::string("击杀计数类型 (") + modeLabel + ")");
		auto root = FindMenuById("root");
		if (root) root->updateSubMenuItemName("kill_count",
			std::string("击杀计数类型 (") + modeLabel + ")");
	}

	// Sync DamageShower GPU profiler overlay labels from loaded config
	{
		auto hitMenu = FindMenuById("hit");
		if (hitMenu) {
			const std::string& currParticle = F::DMGShower.getDmgParticle();
			std::string particleLabel = currParticle.empty() ? "伤害显示样式" : "伤害显示样式 (" + currParticle + ")";
			hitMenu->updateSubMenuItemName("damage", particleLabel);

			int currDist = (int)F::DMGShower.maxDistance;
			hitMenu->updateSubMenuItemName("damage_distance", "伤害显示距离 (" + std::to_string(currDist) + ")");

			int currIntervalMs = static_cast<int>(std::round(F::DMGShower.dmgDisplayInterval * 1000.0f));
			hitMenu->updateSubMenuItemName("damage_interval", "伤害显示间隔 (" + std::to_string(currIntervalMs) + "ms)");
		}
	}

	// check if motion-blur velocity buffer needs clearing
	{
		auto adsMenu = FindMenuById("ads");
		if (adsMenu) {
			adsMenu->setSwitchStateByName("启用ADS", G::Vars.enableAdsSupport);

			// Sync crosshair hide subGPU profiler overlay label
			auto crosshairModeLabel = [](int mode) -> std::string {
				switch (mode) {
					case 0:  return "关";
					case 1:  return "开";
					case 2:  return "自定义";
					default: return "关";
				}
			};
			adsMenu->updateSubMenuItemName("ads_crosshair", "ADS状态隐藏准星 [" + crosshairModeLabel(G::Vars.adsHideCrosshairMode) + "]");

			// Sync per-weapon crosshair pipeline state toggles
			auto crosshairMenu = FindMenuById("ads_crosshair");
			if (crosshairMenu) {
				crosshairMenu->setSwitchStateByName("手枪", G::Vars.adsHideCrosshairPistol);
				crosshairMenu->setSwitchStateByName("双持手枪", G::Vars.adsHideCrosshairPistolDual);
				crosshairMenu->setSwitchStateByName("UZI", G::Vars.adsHideCrosshairUzi);
				crosshairMenu->setSwitchStateByName("木喷", G::Vars.adsHideCrosshairPumpShotgun);
				crosshairMenu->setSwitchStateByName("连喷", G::Vars.adsHideCrosshairAutoShotgun);
				crosshairMenu->setSwitchStateByName("M16", G::Vars.adsHideCrosshairM16A1);
				crosshairMenu->setSwitchStateByName("15连", G::Vars.adsHideCrosshairHuntingRifle);
				crosshairMenu->setSwitchStateByName("MAC10", G::Vars.adsHideCrosshairMac10);
				crosshairMenu->setSwitchStateByName("铁喷", G::Vars.adsHideCrosshairChromeShotgun);
				crosshairMenu->setSwitchStateByName("SCAR", G::Vars.adsHideCrosshairScar);
				crosshairMenu->setSwitchStateByName("30连", G::Vars.adsHideCrosshairMilitarySniper);
				crosshairMenu->setSwitchStateByName("SPAS", G::Vars.adsHideCrosshairSpas);
				crosshairMenu->setSwitchStateByName("榴弹发射器", G::Vars.adsHideCrosshairGrenadeLauncher);
				crosshairMenu->setSwitchStateByName("AK47", G::Vars.adsHideCrosshairAK47);
				crosshairMenu->setSwitchStateByName("沙鹰", G::Vars.adsHideCrosshairDeagle);
				crosshairMenu->setSwitchStateByName("MP5", G::Vars.adsHideCrosshairMP5);
				crosshairMenu->setSwitchStateByName("SG552", G::Vars.adsHideCrosshairSSG552);
				crosshairMenu->setSwitchStateByName("AWP", G::Vars.adsHideCrosshairAWP);
				crosshairMenu->setSwitchStateByName("SCOUT", G::Vars.adsHideCrosshairScout);
				crosshairMenu->setSwitchStateByName("M60", G::Vars.adsHideCrosshairM60);
			}

			auto scopeLabel = [](int mode) -> std::string {
				switch (mode) {
					case 0:  return "关闭";
					case 1:  return "仅ADS";
					case 2:  return "混合";
					default: return "关闭";
				}
			};
			// Scope subGPU profiler overlays are children of ads_scope_weapons, not ads directly
			auto scopeWeaponMenu = FindMenuById("ads_scope_weapons");
			if (scopeWeaponMenu) {
				scopeWeaponMenu->updateSubMenuItemName("ads_ssg552", "SG552 ADS设置 [" + scopeLabel(G::Vars.adsScopeSSG552) + "]");
				scopeWeaponMenu->updateSubMenuItemName("ads_hunting_rifle", "一代连狙ADS设置 [" + scopeLabel(G::Vars.adsScopeHuntingRifle) + "]");
				scopeWeaponMenu->updateSubMenuItemName("ads_military_sniper", "二代连狙ADS设置 [" + scopeLabel(G::Vars.adsScopeMilitarySniper) + "]");
				scopeWeaponMenu->updateSubMenuItemName("ads_scout", "SCOUT ADS设置 [" + scopeLabel(G::Vars.adsScopeScout) + "]");
				scopeWeaponMenu->updateSubMenuItemName("ads_awp", "AWP ADS设置 [" + scopeLabel(G::Vars.adsScopeAWP) + "]");
			}
		}
	}
}
