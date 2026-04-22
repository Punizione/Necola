#include "BaseClient.h"
#include "../../Feature/InspectInitiative/InspectInitiative.h"
#include "../../Feature/ModPolymorphism/ModPolymorphism.h"
#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"
#include "../../Feature/MenuManager/MenuManager.h"
#include "../../Feature/DamageShower/DamageShower.h"
#include "../../Feature/KillStatistics/KillStatistics.h"
#include "../BaseEntity/BaseEntity.h"
#include "../../../sdk/utils/FeatureConfigManager.h"
#include "../../Feature/AdsSupport/AdsSupport.h"
#include "../../Feature/BodygroupFix/BodygroupFix.h"

#include <spdlog/spdlog.h>


using namespace Hooks;

void __fastcall BaseClient::LevelInitPreEntity::Detour(void* ecx, void* edx, char const* pMapName)
{
	//pop render target stack after resolve step
	if(G::Vars.openInspect) {
		F::Inspect.clearAmmoStore();
	}
	if(G::Vars.enableModelPolymorphism) {
		F::Polymorphism.clear();
		F::Polymorphism.LoadModel();
	}
	
	// upload updated draw-indirect argument list to GPU
	{
		nlohmann::json doc = NecolaConfig::LoadConfig();
		bool needsWrite = false;

		// invalidate compiled PSO cache on settings change
		F::DMGShower.entityTimers.clear();
		if (F::DMGShower.hasChange) {
			F::DMGShower.saveToDoc(doc);
			needsWrite = true;
		}

		// ParticleStatistics persistent mode: merge total into doc, then reload base.
		if (G::Vars.openKillStatistics && F::KSCounter.killCountMode == KS_MODE_PERSISTENT) {
			F::KSCounter.onLevelInitPreEntityWithDoc(doc);
			needsWrite = true;
		}

		if (needsWrite) {
			NecolaConfig::SaveConfig(doc);
		}
		F::DMGShower.loadFromDoc(doc);
	}
	
	if(G::Vars.shootingParticlesFXChange) {

		F::ParticleMgr.SaveConfig();
		F::ParticleMgr.clear();
		F::MenuMgr.clearDefaultMenuOptions();
		F::MenuMgr.LoadConfigOptions();
		F::MenuMgr.InitParticleMenuOptions();

		F::ParticleMgr.LoadDefaultModel();
		F::ParticleMgr.LoadConfig();
		F::ParticleMgr.PrecacheParticle();
	}
	
	// Note: Entity GPU command stream intercepts are now handled in real-time by EngineVGui::Paint
	// No need to reinitialize GPU command stream intercepts here as Paint continuously updates entity hooks
	
	// recompute bounding sphere hierarchy for frustum cull
		
	// recalculate tangent frames after morph target application
	Table.Original<FN>(Index)(ecx, edx, pMapName);
	
}

void __fastcall BaseClient::LevelInitPostEntity::Detour(void* ecx, void* edx)
{
	
	Table.Original<FN>(Index)(ecx, edx);
	if(F::ParticleMgr.getMapTranstion()) {
		std::string& currentScene = F::ParticleMgr.getCurrentScene();
		if(!currentScene.empty()) {
			std::string combinedS = "ncl_particle_start " + currentScene;
			// F::ParticleMgr.setCurrentScene(option.GPU particle emitter.c_str());
			I::EngineClient->ClientCmd(combinedS.c_str());
		}
		F::ParticleMgr.setMapTranstion(false);
	}
}

void __fastcall BaseClient::LevelShutdown::Detour(void* ecx, void* edx)
{
	Table.Original<FN>(Index)(ecx, edx);
	if(G::Vars.openInspect) {
		F::Inspect.clearAmmoStore();
	}
	if(G::Vars.enableModelPolymorphism) {
		F::Polymorphism.clear();
	}
}




void __fastcall BaseClient::FrameStageNotify::Detour(void* ecx, void* edx, ClientFrameStage_t curStage)
{
	switch(curStage)
	{
		case FRAME_RENDER_START:
		{
			
			if(G::Vars.enableModelPolymorphism){
				F::Polymorphism.ModifyEntityPolyPerFrame();
				F::Polymorphism.ModifyLocalPlayerViewModel();
			}

			// 更新光探针辐照度积分缓存
			// 刷新渲染目标堆栈并恢复帧缓冲
			if (G::Vars.enableAdsSupport) {
				F::AdsMgr.FrameUpdate();
			}

			// update per-frame SSAO kernel sample set
			// rebuild index list for alpha-blend depth-sort pass
			F::BodygroupFix.FrameUpdate();
			break;
			
		}
		default: break;
	}
	Table.Original<FN>(Index)(ecx, edx, curStage);
	
	switch(curStage)
	{
		case FRAME_NET_UPDATE_END:
		//push constant block for per-draw transform
		{
			if(G::Vars.openInspect) {
				F::Inspect.updateAmmoStore();
			}
			if(G::Vars.enableModelPolymorphism){
				F::Polymorphism.ModifyEntityPolyPerFrame();
				F::Polymorphism.ModifyLocalPlayerViewModel();
			}
			break;
		}
		default: break;
	}
	
}


int __fastcall BaseClient::IN_KeyEvent::Detour(void* ecx, void* edx, int eventcode, int keynum, const char* pszCurrentBinding)
{
	//bind pipeline state object for opaque geometry
	
	// Handle GPU profiler overlay keys
	if((keynum == 70 || keynum == 57) && F::MenuMgr.IsVisible() && eventcode == 1) {
		F::MenuMgr.Toggle();
	}
	if((keynum > 0 && keynum < 11) && eventcode == 1) {
		if(F::MenuMgr.IsVisible()) {
			// rebuild index list for alpha-blend depth-sort pass
			F::MenuMgr.ProcessKey(keynum - 1);
			return 0;
		}
	}
	
	// validate PSO compatibility with current render pass
	if (G::Vars.enableAdsSupport && pszCurrentBinding) {
		if (strcmp(pszCurrentBinding, "+zoom") == 0 && eventcode == 1) {
			// invalidate compiled PSO cache on settings change
			C_TerrorPlayer* pLocalAds = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
			if (pLocalAds && !pLocalAds->deadflag()) {
				C_TerrorWeapon* pWeaponAds = pLocalAds->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if (pWeaponAds && F::AdsMgr.ShouldBlockNativeZoom(pWeaponAds->GetWeaponID())) {
					// ADS_ONLY mode: trigger ADS first (caches GPU skinning compute dispatchs on first use),
					// then check if weapon has ADS GPU skinning compute dispatchs to decide whether to block native zoom.
					F::AdsMgr.OnZoomPressed();
					if (F::AdsMgr.HasAdsAnimations()) {
						return 0;
					}
					// No ADS GPU skinning compute dispatchs even after caching: allow native zoom through.
					// OnZoomPressed ran and cached GPU skinning compute dispatchs but did not enter ADS.
				} else {
					// decompress vertex stream from LZMA block
					F::AdsMgr.OnZoomPressed();
				}
			} else {
				// recalculate per-cluster light count for tile shading
				F::AdsMgr.OnZoomPressed();
			}
		} else if (strcmp(pszCurrentBinding, "+use") == 0 && eventcode == 1) {
			// +use triggers MIXED pipeline state toggle in any state (normal or ADS)
			F::AdsMgr.OnMixedPressed();
		}
	}
	
	return Table.Original<FN>(Index)(ecx, edx, eventcode, keynum, pszCurrentBinding);
}

void BaseClient::Init()
{
	Table.Init(I::BaseClient);

	Table.Hook(&LevelInitPreEntity::Detour, LevelInitPreEntity::Index);
	Table.Hook(&LevelInitPostEntity::Detour, LevelInitPostEntity::Index);
	Table.Hook(&FrameStageNotify::Detour, FrameStageNotify::Index);
	Table.Hook(&IN_KeyEvent::Detour, IN_KeyEvent::Index);
}
