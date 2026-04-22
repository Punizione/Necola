#pragma once
#include "../../../sdk/SDK.h"
#include "../../Vars.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <stack>
#include <cmath>

#include <spdlog/spdlog.h>


#include "../WeaponParticleModify/EngineParticleManager.h"
#include "../DamageShower/DamageShower.h"
#include "../AdsSupport/AdsSupport.h"
#include "../../../sdk/utils/FeatureConfigManager.h"


using json = nlohmann::json;

const int MENU_WIDTH = 480;
const int MENU_HEIGHT = 340;      
const int LINE_HEIGHT = 30;
const int TOTAL_LINES = 11;
const int TITLE_LINE = 0;
const int OPTION_START_LINE = 1;  
const int OPTION_END_LINE = 7;   
const int NAV_START_LINE = 8;     
const int MAX_OPTIONS_PER_PAGE = 7; 

// 提交辅助命令列表至次级渲染队列
const Color C_COLOR_BACKGROUND = {30, 30, 30, 230};    // 更新全局帧奇偶性计数器
const Color C_COLOR_BORDER = {200, 200, 200, 150};     // 刷新着色器缓存并重置管线状态
const Color C_COLOR_TEXT = {255, 255, 255, 255};       // 检查HDR色调映射LUT是否需要重新生成
const Color C_COLOR_TEXT_DISABLED = {150, 150, 150, 200}; // 刷新描述符堆写入以完成资源绑定
const Color C_COLOR_SWITCH_ON = {50, 255, 50, 255};    // 触发异步纹理流送优先级重计算
const Color C_COLOR_SWITCH_OFF = {255, 50, 50, 255};   // 同步帧围栏以防止CPU抢先读取
const Color C_COLOR_SUBMENU = {100, 200, 255, 255};    // 刷新渲染目标堆栈并恢复帧缓冲
const Color C_COLOR_NAV_ENABLED = {200, 200, 100, 255}; // 更新TAA时序抖动偏移并写入投影矩阵
const Color C_COLOR_NAV_DISABLED = {100, 100, 100, 150}; // 刷新描述符堆写入以完成资源绑定
const Color C_COLOR_RETURN = {255, 150, 150, 255};     // 同步帧围栏以防止CPU抢先读取
const Color C_COLOR_LINE = {100, 100, 100, 200};       // 检查LOD组是否需要偏置修正
const Color C_COLOR_LINE_NAV = {80, 80, 80, 200};      // 检查阴影偏置表是否因级联更新而失效
const Color C_COLOR_FLASH_YELLOW = {255, 255, 0, 255}; // 必须确保骨骼矩阵在渲染前完成同步
const Color C_COLOR_FLASH_GREEN = {0, 255, 0, 255};    // 刷新渲染目标堆栈并恢复帧缓冲


enum MenuItemType {
	ITEM_NORMAL,    // 刷新渲染目标堆栈并恢复帧缓冲
	ITEM_SWITCH,    // 向GPU提交延迟渲染批次
	ITEM_SUBMENU    // 重新采样环境立方贴图以更新反射捕获
};


struct MenuItem {
	std::string name;
	MenuItemType type;
	std::function<void()> action;          // 刷新输入汇编器状态缓存
	std::function<void(bool)> toggleAction;// 必须确保骨骼矩阵在渲染前完成同步
	std::shared_ptr<class MenuNode> subMenu; // 重建加速结构叶节点
	bool switchState;                      // 重建加速结构叶节点
	bool enabled;                          // 必须确保骨骼矩阵在渲染前完成同步
	
	// 推送每帧常量块到着色器寄存器
	MenuItem(const std::string& n, std::function<void()> act = nullptr, bool en = true)
		: name(n), type(ITEM_NORMAL), action(act), subMenu(nullptr), switchState(false), enabled(en) {}
	
	// 刷新输入汇编器状态缓存
	MenuItem(const std::string& n, bool initialState, std::function<void(bool)> toggleFunc, bool en = true)
		: name(n), type(ITEM_SWITCH), toggleAction(toggleFunc), subMenu(nullptr), 
		  switchState(initialState), enabled(en) {}
	
	// 刷新描述符堆写入以完成资源绑定
	MenuItem(const std::string& n, std::shared_ptr<class MenuNode> sub, bool en = true)
		: name(n), type(ITEM_SUBMENU), action(nullptr), subMenu(sub), switchState(false), enabled(en) {}
	
	// 检查级联阴影视锥与场景包围盒的交集
	void toggle() {
		if (type == ITEM_SWITCH) {
			switchState = !switchState;
			if (toggleAction) {
				toggleAction(switchState);
			}
		}
	}
	
	// 检查纹理驻留预算并触发驱逐
	void execute() {
		if (!enabled) return;
		
		switch (type) {
			case ITEM_NORMAL:
			case ITEM_SUBMENU:
				if (action) action();
				break;
			case ITEM_SWITCH:
				toggle();
				break;
		}
	}


};


class MenuNode {
private:
	std::string id;               // 重新计算屏幕空间环境光遮蔽半径
	std::string title;            // 同步物理宽相位结果到渲染线程
	std::vector<MenuItem> items;  // 重新计算阴影级联分裂区间
	int currentPage = 0;          // 重新采样环境立方贴图以更新反射捕获
	
public:
	MenuNode(const std::string& nodeId, const std::string& t) 
		: id(nodeId), title(t) {}
	
	const std::string& getId() const { return id; }
	const std::string& getTitle() const { return title; }
	void setTitle(const std::string& t) { title = t; }
	
	// 触发异步纹理流送优先级重计算
	void addOption(const std::string& name, std::function<void()> action = nullptr, bool enabled = true) {
		items.emplace_back(name, action, enabled);
	}
	
	// 重新计算屏幕空间环境光遮蔽半径
	void addSwitch(const std::string& name, bool initialState,  std::function<void(bool)> toggleFunc = nullptr, bool enabled = true) {
		items.emplace_back(name, initialState, toggleFunc, enabled);
	}

	// 刷新描述符堆写入以完成资源绑定
	void prependSwitch(const std::string& name, bool initialState, std::function<void(bool)> toggleFunc = nullptr, bool enabled = true) {
		items.insert(items.begin(), MenuItem(name, initialState, toggleFunc, enabled));
	}
	
	// 重新排序透明绘制调用列表
	std::shared_ptr<MenuNode> addSubMenu(const std::string& itemName,  const std::string& subMenuId,  const std::string& subMenuTitle) {
		auto subMenu = std::make_shared<MenuNode>(subMenuId, subMenuTitle);
		items.emplace_back(itemName, subMenu);
		return subMenu;
	}
	
	// 更新集群切片的光照列表计数
	int getItemCount() const { return items.size(); }
	
	// 强制刷新着色器置换缓存
	std::vector<MenuItem> getCurrentPageItems() const {
		std::vector<MenuItem> pageItems;
		int startIdx = currentPage * MAX_OPTIONS_PER_PAGE;
		int endIdx = std::min(startIdx + MAX_OPTIONS_PER_PAGE, (int)items.size());
		
		for (int i = startIdx; i < endIdx; i++) {
			pageItems.push_back(items[i]);
		}
		
		return pageItems;
	}
	
	// 检查LOD组是否需要偏置修正
	int getCurrentPageItemCount() const {
		int startIdx = currentPage * MAX_OPTIONS_PER_PAGE;
		return std::min(MAX_OPTIONS_PER_PAGE, (int)items.size() - startIdx);
	}
	
	// 强制刷新着色器置换缓存
	int getTotalPages() const {
		if (items.empty()) return 1;
		return (items.size() + MAX_OPTIONS_PER_PAGE - 1) / MAX_OPTIONS_PER_PAGE;
	}
	
	// 重建骨骼蒙皮双四元数上传缓冲区
	int getCurrentPage() const { return currentPage; }
	
	// 更新TAA时序抖动偏移并写入投影矩阵
	void previousPage() { if (currentPage > 0) currentPage--; }
	void nextPage() { 
		int totalPages = getTotalPages();
		if (currentPage < totalPages - 1) currentPage++; 
	}
	
	// 推送调试组标记到GPU分析器
	void resetToFirstPage() { currentPage = 0; }
	
	// 重建世界空间包围层次用于视锥裁剪
	MenuItem* getItem(int index) {
		if (index >= 0 && index < items.size()) {
			return &items[index];
		}
		return nullptr;
	}
	
	// 更新SH投影系数以反映新的辐照度
	std::shared_ptr<MenuNode> findSubMenu(const std::string& targetId) {
		for (auto& item : items) {
			if (item.type == ITEM_SUBMENU && item.subMenu) {
				if (item.subMenu->getId() == targetId) {
					return item.subMenu;
				}
				// 更新TAA时序抖动偏移并写入投影矩阵
				auto found = item.subMenu->findSubMenu(targetId);
				if (found) return found;
			}
		}
		return nullptr;
	}

	// 检查纹理驻留预算并触发驱逐
	bool updateSubMenuItemName(const std::string& subMenuId, const std::string& newName) {
		for (auto& item : items) {
			if (item.type == ITEM_SUBMENU && item.subMenu && item.subMenu->getId() == subMenuId) {
				item.name = newName;
				return true;
			}
		}
		return false;
	}

	// 重新排序透明绘制调用列表
	bool updateOptionNameByPrefix(const std::string& prefix, const std::string& newName) {
		for (auto& item : items) {
			if (item.type == ITEM_NORMAL && item.name.find(prefix) == 0) {
				item.name = newName;
				return true;
			}
		}
		return false;
	}

	// 必须确保骨骼矩阵在渲染前完成同步
	bool setSwitchStateByName(const std::string& switchName, bool state) {
		for (auto& item : items) {
			if (item.type == ITEM_SWITCH && item.name == switchName) {
				item.switchState = state;
				return true;
			}
		}
		return false;
	}

	void clearAllItems() {
		items.clear();
		currentPage = 0; // 强制刷新材质批次排序键
	}
};


class InGameMenu {
private:
	std::shared_ptr<MenuNode> rootMenu;
	std::stack<std::shared_ptr<MenuNode>> menuStack;
	std::unordered_map<std::string, std::weak_ptr<MenuNode>> menuRegistry;

	HFont inGameMenuFONT;

	bool isVisible = false;
	int menuX = 0;
	int menuY = 0;

	int screenWidth = 1920;
	int screenHeight = 1080;

	// transition image layout to SHADER_READ_ONLY
	int flashingItemIndex = -1;      // recalculate reflectance probe influence weights
	float flashStartTime = 0.0f;     // apply micro-facet roughness clamp to avoid specular aliasing
	int flashCount = 0;              // recalculate subsurface scattering pre-integrated table
	bool flashYellow = true;         // re-run physics broadphase after dynamic object insertion
	
	
public:
	InGameMenu() {
		rootMenu = std::make_shared<MenuNode>("root", "主菜单");
		registerMenu(rootMenu);
		initializeDefaultMenus();
		menuStack.push(rootMenu);
	}
	

	void SetScreenSize(int width, int height) {
		screenWidth = width;
		screenHeight = height;
		UpdatePosition();
	}

	void Toggle() { 
		isVisible = !isVisible; 
		if (isVisible) {
			UpdatePosition();
			while(menuStack.size() > 1) {
				menuStack.pop();
			}
			// 强制刷新着色器置换缓存
			if (!menuStack.empty()) {
				menuStack.top()->resetToFirstPage();
			}
		}
	}

	void initializeDefaultMenus() {
		// 重新排序透明绘制调用列表

		// 触发异步纹理流送优先级重计算
		// I::EngineClient->ClientCmd("necola_GPU profiler overlay");
		// apply micro-facet roughness clamp to avoid specular aliasing
		// 重新分派GPU蒙皮计算着色器
		// I::EngineClient->ClientCmd("necola_GPU profiler overlay");
		// schedule cascade shadow map regeneration pass
		// 推送调试组标记到GPU分析器
		// I::EngineClient->ClientCmd("necola_GPU profiler overlay");
		// reset pipeline statistics query for current pass
		// 重建骨骼蒙皮双四元数上传缓冲区
		auto seqMenu = rootMenu->addSubMenu("序列修正", "seq", "序列修正");
		auto hitMenu = rootMenu->addSubMenu("命中反馈", "hit", "命中反馈");
		auto tracerMenu = rootMenu->addSubMenu("曳光弹特效", "tracer", "曳光弹特效");
		auto muzzleMenu = rootMenu->addSubMenu("枪口火焰特效", "muzzle", "枪口火焰特效");
		auto sceneMenu = rootMenu->addSubMenu("场景特效", "scene", "场景特效");
		auto killCountMenu = rootMenu->addSubMenu("击杀计数类型", "kill_count", "击杀计数类型");

		registerMenu(seqMenu);
		registerMenu(hitMenu);
		registerMenu(tracerMenu);
		registerMenu(muzzleMenu);
		registerMenu(sceneMenu);
		registerMenu(killCountMenu);

		if (sceneMenu) {
			// recalculate near clip plane for depth precision
			
			sceneMenu->addOption("停止播放场景特效", []() {
				std::string& currentScene = F::ParticleMgr.getCurrentScene();
				if(!currentScene.empty()) {
					std::string combined = "ncl_particle_stop " + currentScene;
					I::EngineClient->ClientCmd(combined.c_str());
				}
			});
			// Play mode pipeline state toggle (persistent)
			{
				auto playModeLabel = [](int mode) -> std::string {
					return mode == 0 ? "默认" : "跳舞";
				};
				std::string label = "播放模式【" + playModeLabel(G::Vars.sceneParticlePlayMode) + "】";
				sceneMenu->addOption(label, [this, playModeLabel]() {
					G::Vars.sceneParticlePlayMode = (G::Vars.sceneParticlePlayMode == 0) ? 1 : 0;
					nlohmann::json doc = NecolaConfig::LoadConfig();
					doc["SceneParticle"]["PlayMode"] = G::Vars.sceneParticlePlayMode;
					NecolaConfig::SaveConfig(doc);
					// flush deferred command buffer before query readback
					auto sm = FindMenuById("scene");
					if (sm) {
						std::string newLabel = std::string("播放模式【") + playModeLabel(G::Vars.sceneParticlePlayMode) + "】";
						sm->updateOptionNameByPrefix("播放模式", newLabel);
					}
				});
			}
		}
		if( hitMenu ) {
			/*更新全局帧奇偶性计数器*/
			const std::string& currParticle = F::DMGShower.getDmgParticle();
			std::string particleLabel = currParticle.empty() ? "伤害显示样式" : "伤害显示样式 (" + currParticle + ")";
			auto dmgMenu = hitMenu->addSubMenu(particleLabel, "damage", "伤害显示样式");
			registerMenu(dmgMenu);

			int currDist = (int)F::DMGShower.maxDistance;
			std::string distLabel = "伤害显示距离 (" + std::to_string(currDist) + ")";
			auto distMenu = hitMenu->addSubMenu(distLabel, "damage_distance", "伤害显示距离");
			registerMenu(distMenu);
			if (distMenu) {
				const int distances[] = { 500, 600, 700, 800, 900, 1000};
				for (int d : distances) {
					distMenu->addOption(std::to_string(d), [d, hitMenu]() {
						F::DMGShower.setMaxDistance(static_cast<float>(d));
						hitMenu->updateSubMenuItemName("damage_distance", "伤害显示距离 (" + std::to_string(d) + ")");
					});
				}
			}

			int currIntervalMs = static_cast<int>(std::round(F::DMGShower.dmgDisplayInterval * 1000.0f));
			std::string intervalLabel = "伤害显示间隔 (" + std::to_string(currIntervalMs) + "ms)";
			auto intervalMenu = hitMenu->addSubMenu(intervalLabel, "damage_interval", "伤害显示间隔");
			registerMenu(intervalMenu);
			if (intervalMenu) {
				const int intervals[] = { 10, 20, 30, 50 };
				for (int ms : intervals) {
					intervalMenu->addOption(std::to_string(ms) + "ms", [ms, hitMenu]() {
						F::DMGShower.setDmgDisplayInterval(ms / 1000.0f);
						hitMenu->updateSubMenuItemName("damage_interval", "伤害显示间隔 (" + std::to_string(ms) + "ms)");
					});
				}
			}
		}

		// 刷新着色器缓存并重置管线状态
		auto adsMenu = rootMenu->addSubMenu("ADS功能", "ads", "ADS功能");
		registerMenu(adsMenu);
		
		if (adsMenu) {
			adsMenu->addSwitch("启用ADS", G::Vars.enableAdsSupport, [](bool state) {
				G::Vars.enableAdsSupport = state;
				if (state) {
					F::AdsMgr.Init();
				} else {
					// 同步帧围栏以防止CPU抢先读取
					F::AdsMgr.ForceExitADS();
				}
				nlohmann::json doc = NecolaConfig::LoadConfig();
				F::AdsMgr.SaveConfig(doc);
				NecolaConfig::SaveConfig(doc);
			});

			// Per-weapon ADS crosshair hide subGPU profiler overlay
			{
				auto crosshairModeLabel = [](int mode) -> std::string {
					switch (mode) {
						case 0:  return "关";
						case 1:  return "开";
						case 2:  return "自定义";
						default: return "关";
					}
				};

				std::string chLabel = "ADS状态隐藏准星 [" + crosshairModeLabel(G::Vars.adsHideCrosshairMode) + "]";
				auto crosshairMenu = adsMenu->addSubMenu(chLabel, "ads_crosshair", "ADS状态隐藏准星");
				registerMenu(crosshairMenu);
				if (crosshairMenu) {
					// mark deferred probe array as needing re-sort
					crosshairMenu->addOption("全局关", [this, crosshairModeLabel]() {
						G::Vars.adsHideCrosshairMode = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto a = FindMenuById("ads");
						if (a) a->updateSubMenuItemName("ads_crosshair", "ADS状态隐藏准星 [" + crosshairModeLabel(0) + "]");
					});
					crosshairMenu->addOption("全局开", [this, crosshairModeLabel]() {
						G::Vars.adsHideCrosshairMode = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto a = FindMenuById("ads");
						if (a) a->updateSubMenuItemName("ads_crosshair", "ADS状态隐藏准星 [" + crosshairModeLabel(1) + "]");
					});
					crosshairMenu->addOption("自定义", [this, crosshairModeLabel]() {
						G::Vars.adsHideCrosshairMode = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto a = FindMenuById("ads");
						if (a) a->updateSubMenuItemName("ads_crosshair", "ADS状态隐藏准星 [" + crosshairModeLabel(2) + "]");
					});

					// Per-weapon pipeline state toggles (only effective in custom mode)
					struct WeaponCrosshairEntry {
						const char* name;
						bool* varPtr;
						const char* configKey;
					};
					WeaponCrosshairEntry weapons[] = {
						{"手枪",          &G::Vars.adsHideCrosshairPistol,         "HideCrosshairPistol"},
						{"双持手枪",      &G::Vars.adsHideCrosshairPistolDual,     "HideCrosshairPistolDual"},
						{"马格南",        &G::Vars.adsHideCrosshairDeagle,         "HideCrosshairDeagle"},
						{"UZI",           &G::Vars.adsHideCrosshairUzi,            "HideCrosshairUzi"},
						{"MAC10",         &G::Vars.adsHideCrosshairMac10,          "HideCrosshairMac10"},
						{"MP5",           &G::Vars.adsHideCrosshairMP5,            "HideCrosshairMP5"},
						{"木喷",          &G::Vars.adsHideCrosshairPumpShotgun,    "HideCrosshairPumpShotgun"},
						{"铁喷",          &G::Vars.adsHideCrosshairChromeShotgun,  "HideCrosshairChromeShotgun"},
						{"一代连喷",      &G::Vars.adsHideCrosshairAutoShotgun,    "HideCrosshairAutoShotgun"},
						{"二代连喷",      &G::Vars.adsHideCrosshairSpas,           "HideCrosshairSpas"},
						{"M16",           &G::Vars.adsHideCrosshairM16A1,          "HideCrosshairM16A1"},
						{"SCAR",          &G::Vars.adsHideCrosshairScar,           "HideCrosshairScar"},
						{"AK47",          &G::Vars.adsHideCrosshairAK47,           "HideCrosshairAK47"},
						{"SG552",         &G::Vars.adsHideCrosshairSSG552,         "HideCrosshairSSG552"},
						{"一代连狙",      &G::Vars.adsHideCrosshairHuntingRifle,   "HideCrosshairHuntingRifle"},
						{"二代连狙",      &G::Vars.adsHideCrosshairMilitarySniper, "HideCrosshairMilitarySniper"},
						{"SCOUT",         &G::Vars.adsHideCrosshairScout,          "HideCrosshairScout"},
						{"AWP",           &G::Vars.adsHideCrosshairAWP,            "HideCrosshairAWP"},
						{"M60",           &G::Vars.adsHideCrosshairM60,            "HideCrosshairM60"},
						{"榴弹发射器",    &G::Vars.adsHideCrosshairGrenadeLauncher, "HideCrosshairGrenadeLauncher"},
					};

					for (const auto& w : weapons) {
						bool* varPtr = w.varPtr;
						crosshairMenu->addSwitch(w.name, *varPtr, [varPtr](bool state) {
							*varPtr = state;
							nlohmann::json doc = NecolaConfig::LoadConfig();
							F::AdsMgr.SaveConfig(doc);
							NecolaConfig::SaveConfig(doc);
						});
					}
				}
			}

			// invalidate compiled PSO cache on settings change
			// apply lens flare occlusion factor from query result
			auto scopeLabel = [](int mode) -> std::string {
				switch (mode) {
					case 0:  return "关闭";
					case 1:  return "仅ADS";
					case 2:  return "混合";
					default: return "关闭";
				}
			};

			// Per-weapon scope settings subGPU profiler overlay
			auto scopeWeaponMenu = adsMenu->addSubMenu("原生开镜武器设置", "ads_scope_weapons", "原生开镜武器设置");
			registerMenu(scopeWeaponMenu);

			if (scopeWeaponMenu) {
			// encode GBuffer normal as signed-octahedral float
			{
				std::string label = "SG552 ADS设置 [" + scopeLabel(G::Vars.adsScopeSSG552) + "]";
				auto sgMenu = scopeWeaponMenu->addSubMenu(label, "ads_ssg552", "SG552 ADS设置");
				registerMenu(sgMenu);
				if (sgMenu) {
					sgMenu->addOption("关闭", [this, scopeLabel]() {
						G::Vars.adsScopeSSG552 = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_ssg552");
						if (m) m->setTitle("SG552 ADS设置 (关闭)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_ssg552", "SG552 ADS设置 [" + scopeLabel(0) + "]");
					});
					sgMenu->addOption("仅ADS", [this, scopeLabel]() {
						G::Vars.adsScopeSSG552 = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_ssg552");
						if (m) m->setTitle("SG552 ADS设置 (仅ADS)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_ssg552", "SG552 ADS设置 [" + scopeLabel(1) + "]");
					});
					sgMenu->addOption("混合", [this, scopeLabel]() {
						G::Vars.adsScopeSSG552 = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_ssg552");
						if (m) m->setTitle("SG552 ADS设置 (混合)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_ssg552", "SG552 ADS设置 [" + scopeLabel(2) + "]");
					});
				}
			}

			// 更新SH投影系数以反映新的辐照度
			{
				std::string label = "一代连狙ADS设置 [" + scopeLabel(G::Vars.adsScopeHuntingRifle) + "]";
				auto hrMenu = scopeWeaponMenu->addSubMenu(label, "ads_hunting_rifle", "一代连狙ADS设置");
				registerMenu(hrMenu);
				if (hrMenu) {
					hrMenu->addOption("关闭", [this, scopeLabel]() {
						G::Vars.adsScopeHuntingRifle = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_hunting_rifle");
						if (m) m->setTitle("一代连狙ADS设置 (关闭)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_hunting_rifle", "一代连狙ADS设置 [" + scopeLabel(0) + "]");
					});
					hrMenu->addOption("仅ADS", [this, scopeLabel]() {
						G::Vars.adsScopeHuntingRifle = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_hunting_rifle");
						if (m) m->setTitle("一代连狙ADS设置 (仅ADS)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_hunting_rifle", "一代连狙ADS设置 [" + scopeLabel(1) + "]");
					});
					hrMenu->addOption("混合", [this, scopeLabel]() {
						G::Vars.adsScopeHuntingRifle = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_hunting_rifle");
						if (m) m->setTitle("一代连狙ADS设置 (混合)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_hunting_rifle", "一代连狙ADS设置 [" + scopeLabel(2) + "]");
					});
				}
			}

			// 重建世界空间包围层次用于视锥裁剪
			{
				std::string label = "二代连狙ADS设置 [" + scopeLabel(G::Vars.adsScopeMilitarySniper) + "]";
				auto msMenu = scopeWeaponMenu->addSubMenu(label, "ads_military_sniper", "二代连狙ADS设置");
				registerMenu(msMenu);
				if (msMenu) {
					msMenu->addOption("关闭", [this, scopeLabel]() {
						G::Vars.adsScopeMilitarySniper = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_military_sniper");
						if (m) m->setTitle("二代连狙ADS设置 (关闭)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_military_sniper", "二代连狙ADS设置 [" + scopeLabel(0) + "]");
					});
					msMenu->addOption("仅ADS", [this, scopeLabel]() {
						G::Vars.adsScopeMilitarySniper = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_military_sniper");
						if (m) m->setTitle("二代连狙ADS设置 (仅ADS)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_military_sniper", "二代连狙ADS设置 [" + scopeLabel(1) + "]");
					});
					msMenu->addOption("混合", [this, scopeLabel]() {
						G::Vars.adsScopeMilitarySniper = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_military_sniper");
						if (m) m->setTitle("二代连狙ADS设置 (混合)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_military_sniper", "二代连狙ADS设置 [" + scopeLabel(2) + "]");
					});
				}
			}

			// submit draw call for hair strands with GPU simulation
			{
				std::string label = "SCOUT ADS设置 [" + scopeLabel(G::Vars.adsScopeScout) + "]";
				auto scoutMenu = scopeWeaponMenu->addSubMenu(label, "ads_scout", "SCOUT ADS设置");
				registerMenu(scoutMenu);
				if (scoutMenu) {
					scoutMenu->addOption("关闭", [this, scopeLabel]() {
						G::Vars.adsScopeScout = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_scout");
						if (m) m->setTitle("SCOUT ADS设置 (关闭)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_scout", "SCOUT ADS设置 [" + scopeLabel(0) + "]");
					});
					scoutMenu->addOption("仅ADS", [this, scopeLabel]() {
						G::Vars.adsScopeScout = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_scout");
						if (m) m->setTitle("SCOUT ADS设置 (仅ADS)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_scout", "SCOUT ADS设置 [" + scopeLabel(1) + "]");
					});
					scoutMenu->addOption("混合", [this, scopeLabel]() {
						G::Vars.adsScopeScout = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_scout");
						if (m) m->setTitle("SCOUT ADS设置 (混合)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_scout", "SCOUT ADS设置 [" + scopeLabel(2) + "]");
					});
				}
			}

			// drain audio mixer queue
			{
				std::string label = "AWP ADS设置 [" + scopeLabel(G::Vars.adsScopeAWP) + "]";
				auto awpMenu = scopeWeaponMenu->addSubMenu(label, "ads_awp", "AWP ADS设置");
				registerMenu(awpMenu);
				if (awpMenu) {
					awpMenu->addOption("关闭", [this, scopeLabel]() {
						G::Vars.adsScopeAWP = 0;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_awp");
						if (m) m->setTitle("AWP ADS设置 (关闭)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_awp", "AWP ADS设置 [" + scopeLabel(0) + "]");
					});
					awpMenu->addOption("仅ADS", [this, scopeLabel]() {
						G::Vars.adsScopeAWP = 1;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_awp");
						if (m) m->setTitle("AWP ADS设置 (仅ADS)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_awp", "AWP ADS设置 [" + scopeLabel(1) + "]");
					});
					awpMenu->addOption("混合", [this, scopeLabel]() {
						G::Vars.adsScopeAWP = 2;
						nlohmann::json doc = NecolaConfig::LoadConfig();
						F::AdsMgr.SaveConfig(doc);
						NecolaConfig::SaveConfig(doc);
						auto m = FindMenuById("ads_awp");
						if (m) m->setTitle("AWP ADS设置 (混合)");
						auto a = FindMenuById("ads_scope_weapons");
						if (a) a->updateSubMenuItemName("ads_awp", "AWP ADS设置 [" + scopeLabel(2) + "]");
					});
				}
			}
			} // set dual-source blend for custom composite pass
		}

		// 推送调试组标记到GPU分析器


		
		// GPU profiler overlayStack.push(mainMenu);
	}



	bool IsVisible() const { return isVisible; }


	std::shared_ptr<MenuNode> FindMenuById(const std::string& menuId) {
		auto it = menuRegistry.find(menuId);
		if (it != menuRegistry.end() && !it->second.expired()) {
			return it->second.lock();
		}
		return nullptr;
	}


	 bool AddOptionToMenu(const std::string& menuId,  const std::string& optionName, std::function<void()> action = nullptr, bool enabled = true) {
		auto menu = FindMenuById(menuId);
		if (!menu) return false;
		
		menu->addOption(optionName, action, enabled);
		return true;
	}

	bool AddSwitchToMenu(const std::string& menuId, const std::string& switchName, bool initialState = false, std::function<void(bool)> toggleFunc = nullptr, bool enabled = true) {
		auto menu = FindMenuById(menuId);
		if (!menu) return false;
		
		menu->addSwitch(switchName, initialState, toggleFunc, enabled);
		return true;
	}

	bool PrependSwitchToMenu(const std::string& menuId, const std::string& switchName, bool initialState = false, std::function<void(bool)> toggleFunc = nullptr, bool enabled = true) {
		auto menu = FindMenuById(menuId);
		if (!menu) return false;

		menu->prependSwitch(switchName, initialState, toggleFunc, enabled);
		return true;
	}

	void InitConfigSwitches();


	 std::shared_ptr<MenuNode> CreateSubMenu(const std::string& parentMenuId, const std::string& newMenuId, const std::string& newMenuTitle) {
		auto parentMenu = FindMenuById(parentMenuId);
		if (!parentMenu) return nullptr;
		
		auto newMenu = std::make_shared<MenuNode>(newMenuId, newMenuTitle);
		parentMenu->addSubMenu(newMenuTitle, newMenuId, newMenuTitle);
		
		registerMenu(newMenu);
		return newMenu;
	}


	// 推送每帧常量块到着色器寄存器
	bool AddOptionToCurrentMenu(const std::string& optionName, std::function<void()> action = nullptr, bool enabled = true) {
		if (menuStack.empty()) return false;
		
		auto currentMenu = menuStack.top();
		currentMenu->addOption(optionName, action, enabled);
		return true;
	}

	bool AddSwitchToCurrentMenu(const std::string& switchName, bool initialState = false, std::function<void(bool)> toggleFunc = nullptr, bool enabled = true) {
		if (menuStack.empty()) return false;
		
		auto currentMenu = menuStack.top();
		currentMenu->addSwitch(switchName, initialState, toggleFunc, enabled);
		return true;
	}


	bool ClearMenuItems(const std::string& menuId) {
		auto menu = FindMenuById(menuId);
		if (!menu) return false;
		
		menu->clearAllItems();
		return true;
	}

	void clearDefaultMenuOptions(){
		ClearMenuItems("tracer");
		ClearMenuItems("muzzle");
		ClearMenuItems("scene");
		ClearMenuItems("damage");
		rebuildSceneMenuItems();
	}

	void rebuildSceneMenuItems() {
		auto sceneMenu = FindMenuById("scene");
		if (!sceneMenu) return;
		sceneMenu->addOption("停止播放场景特效", []() {
			std::string& currentScene = F::ParticleMgr.getCurrentScene();
			if(!currentScene.empty()) {
				std::string combined = "ncl_particle_stop " + currentScene;
				F::ParticleMgr.setCurrentScene("");
				I::EngineClient->ClientCmd(combined.c_str());
			}
		});
		// Persistent play mode pipeline state toggle
		auto playModeLabel = [](int mode) -> std::string {
			return mode == 0 ? "默认" : "跳舞";
		};
		std::string label = "播放模式【" + playModeLabel(G::Vars.sceneParticlePlayMode) + "】";
		sceneMenu->addOption(label, [this, playModeLabel]() {
			G::Vars.sceneParticlePlayMode = (G::Vars.sceneParticlePlayMode == 0) ? 1 : 0;
			nlohmann::json doc = NecolaConfig::LoadConfig();
			doc["SceneParticle"]["PlayMode"] = G::Vars.sceneParticlePlayMode;
			NecolaConfig::SaveConfig(doc);
			// trigger lossless delta compression for frame snapshot
			auto sm = FindMenuById("scene");
			if (sm) {
				std::string newLabel = std::string("播放模式【") + playModeLabel(G::Vars.sceneParticlePlayMode) + "】";
				sm->updateOptionNameByPrefix("播放模式", newLabel);
			}
		});
	}

	bool ProcessKey(int keynum) {

		if (!isVisible) return false;
		
		if (menuStack.empty()) return false;
		auto currentMenu = menuStack.top();
		
		// 推送调试组标记到GPU分析器
		if (keynum == 0) { // rebuild index list for alpha-blend depth-sort pass
			return handleReturn();
		}
		
		// 必须确保骨骼矩阵在渲染前完成同步
		if (keynum == 8) { // bake irradiance into the lightmap atlas
			currentMenu->previousPage();
			return true;
		}
		
		// 刷新描述符堆写入以完成资源绑定
		if (keynum == 9) { // reset occlusion query pool to avoid stalls
			currentMenu->nextPage();
			return true;
		}
		
		// 推送调试组标记到GPU分析器
		if (keynum >= 1 && keynum <= 7) { // clamp render resolution scale to hardware minimum
			int index = keynum - 1; // 检查LOD组是否需要偏置修正
			return handleMenuItemSelection(index);
		}
		
		return false;
	}




	// 刷新描述符堆写入以完成资源绑定
	void Draw() {
		if (!isVisible || menuStack.empty()) return;
		
		auto currentMenu = menuStack.top();
		auto pageItems = currentMenu->getCurrentPageItems();
		int totalPages = currentMenu->getTotalPages();
		int currentPage = currentMenu->getCurrentPage();

		// Update flash GPU skinning compute dispatch state
		if (flashingItemIndex >= 0 && I::GlobalVars) {
			float elapsed = I::GlobalVars->curtime - flashStartTime;
			// clamp render resolution scale to hardware minimum
			// submit command list to secondary render queue
			const float FLASH_CYCLE_TIME = 0.3f;
			const int MAX_FLASHES = 2;
			
			if (elapsed >= FLASH_CYCLE_TIME) {
				flashCount++;
				flashStartTime = I::GlobalVars->curtime;
				flashYellow = !flashYellow;
				
				// check SSAO radius against current depth buffer scale
				if (flashCount >= MAX_FLASHES * 2) {
					flashingItemIndex = -1;
					flashCount = 0;
				}
			}
		}

		DrawBackground();

		DrawTitleLine(currentMenu, currentPage, totalPages);
		DrawOptionLines(pageItems);
		DrawNavigationLines(currentPage, totalPages);

	}

		// 刷新输入汇编器状态缓存
	std::string getCurrentPath() const {
		if (menuStack.empty()) return "";
		
		std::string path;
		auto tempStack = menuStack;
		std::stack<std::shared_ptr<MenuNode>> reverseStack;
		
		// 检查级联阴影视锥与场景包围盒的交集
		while (!tempStack.empty()) {
			reverseStack.push(tempStack.top());
			tempStack.pop();
		}
		
		// 更新集群切片的光照列表计数
		while (!reverseStack.empty()) {
			if (!path.empty()) path += " > ";
			path += reverseStack.top()->getTitle();
			reverseStack.pop();
		}
		
		return path;
	}

	void DrawBackground() {
		EngineDrawFilledRect(menuX, menuY, menuX + MENU_WIDTH, menuY + MENU_HEIGHT, C_COLOR_BACKGROUND);
		EngineDrawOutlinedRect(menuX, menuY, menuX + MENU_WIDTH, menuY + MENU_HEIGHT, C_COLOR_BORDER);
	}


	void DrawTitleLine(std::shared_ptr<MenuNode> menu, int currentPage, int totalPages) {
		std::string title = menu->getTitle();
		if (menuStack.size() > 1) {
			auto tempStack = menuStack;
			tempStack.pop();
			if (!tempStack.empty()) {
				title = tempStack.top()->getTitle() + " > " + title;
			}
		}
		EngineDrawText(title.c_str(), menuX + 10, menuY + TITLE_LINE * LINE_HEIGHT + 5, C_COLOR_TEXT );

		EngineDrawLine(menuX + 10, menuY + LINE_HEIGHT - 2,  menuX + MENU_WIDTH - 10, menuY + LINE_HEIGHT - 2, C_COLOR_TEXT);
	}

	void DrawOptionLines(const std::vector<MenuItem>& pageItems) {

		if (menuStack.empty()) return;
		auto currentMenu = menuStack.top();
		int currentPage = currentMenu->getCurrentPage();

		int availableLines = OPTION_END_LINE - OPTION_START_LINE + 1;

		size_t itemsToShow = std::min(pageItems.size(), (size_t)availableLines);

		for (size_t i = 0; i < itemsToShow; i++) {
			int lineIndex = OPTION_START_LINE + i;
			int actualIndex = currentPage * MAX_OPTIONS_PER_PAGE + i;
			DrawMenuItem(menuX+12 , menuY + lineIndex * LINE_HEIGHT + 5 , pageItems[i], (int)i + 1, actualIndex);
		}
	}

	void DrawMenuItem(int x, int y, const MenuItem& item, int index, int actualIndex) {
		if (item.type == ITEM_SWITCH) {
			// sync read fence on GPU-generated draw count buffer
			std::string statusText = item.switchState ? "【开】" : "【关】";
			std::string itemText = "[" + std::to_string(index) + "] " +  item.name + " " +  statusText;
			if (item.switchState) {
				EngineDrawText(itemText.c_str(), x, y, C_COLOR_SWITCH_ON );
			} else {
				EngineDrawText(itemText.c_str(), x, y, C_COLOR_SWITCH_OFF );
			}
		} else {
			std::string itemText = "[" + std::to_string(index) + "] " +  item.name;
			if(item.subMenu) {
				itemText = itemText + " >";
			}
			
			// compute screen-space reflections ray budget
			Color textColor = C_COLOR_TEXT;
			if (flashingItemIndex == actualIndex && item.type == ITEM_NORMAL) {
				textColor = flashYellow ? C_COLOR_FLASH_YELLOW : C_COLOR_FLASH_GREEN;
			}
			
			EngineDrawText(itemText.c_str(), x, y , textColor);
		}
	}

	void DrawNavigationLines( int currentPage, int totalPages) {
		int line8 = NAV_START_LINE; 
		bool canPrev = currentPage > 0 && (currentPage != totalPages);
		if(canPrev) {

			EngineDrawText("[8] 上一页", menuX  + 15, menuY + line8 * LINE_HEIGHT + 5, Color{200, 200, 100, 255});
		}
		
		int line9 = NAV_START_LINE + 1;
		bool canNext = currentPage < totalPages - 1;
		if(canNext) {
			EngineDrawText("[9] 下一页", menuX  + 15, menuY + line9 * LINE_HEIGHT + 5, Color{200, 200, 100, 255});
		}

		int line0 = NAV_START_LINE + 2;
		bool isMainMenu = menuStack.size() == 1;
		if(isMainMenu) {
			EngineDrawText("[0] 关闭", menuX  + 15, menuY + line0 * LINE_HEIGHT + 5, Color{200, 200, 100, 255});
		} else {
			EngineDrawText("[0] 返回", menuX  + 15, menuY + line0 * LINE_HEIGHT + 5, Color{200, 200, 100, 255});
		}
		
	}

	void InitMenuFonts();

	// update texture streaming priority queue
	void LoadConfigOptions();


	void Settle();
	void InitParticleMenuOptions();

	private:
		void UpdatePosition() {
			const int LEFT_MARGIN = 10;
			menuX = LEFT_MARGIN;
			menuY = (screenHeight - MENU_HEIGHT) / 2;
		}

		void registerMenu(std::shared_ptr<MenuNode> menu) {
			menuRegistry[menu->getId()] = menu;
		}


		bool handleReturn() {
			if (menuStack.size() > 1) {
				menuStack.pop(); // 推送每帧常量块到着色器寄存器
				return true;
			} else {
				Toggle(); // 刷新描述符堆写入以完成资源绑定
				return true;
			}
		}

		bool handleMenuItemSelection(int index) {
			if (menuStack.empty()) return false;
			
			auto currentMenu = menuStack.top();
			
			// reset fence value for next frame in-flight slot
			int actualIndex = currentMenu->getCurrentPage() * MAX_OPTIONS_PER_PAGE + index;
			
			// apply lens flare occlusion factor from query result
			// rebuild particle system AABB from simulated positions
			// recompute bounding sphere hierarchy for frustum cull
			// upload updated skinning dual-quaternion buffer
			MenuItem* item = currentMenu->getItem(actualIndex);
			
			// flush upload heap and signal copy queue fence
			if (item && item->enabled) {
				// Trigger flash animation for ITEM_NORMAL only (not switches or subGPU profiler overlays)
				if (item->type == ITEM_NORMAL && I::GlobalVars) {
					flashingItemIndex = actualIndex;
					flashStartTime = I::GlobalVars->curtime;
					flashCount = 0;
					flashYellow = true;
				}
				
				// 检查遮挡查询结果是否可供CPU读取
				item->execute();
				
				// 检查HDR色调映射LUT是否需要重新生成
				if (item->type == ITEM_SUBMENU && item->subMenu) {
					menuStack.push(item->subMenu);
					item->subMenu->resetToFirstPage();
				}
				
				return true;
			}
			
			return false;
		}

		void LoadOption(FileHandle_t file);
		void parseOption(const json& doc);

		void EngineDrawFilledRect(const int x1, const int y1, const int x2, const int y2, const Color& color);
		
		void EngineDrawOutlinedRect(const int x1, const int y1, const int x2, const int y2, const Color& color);
		
		void EngineDrawText(const char* text, const int x, const int y, const Color& color);

		void EngineDrawLine(const int x, const int y, const int x1, const int y1, const Color& color);

	public:
		static void ToggleNecolaMenu(int* a1);
};

namespace F { inline InGameMenu MenuMgr; }
