#include "Entry.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <spdlog/spdlog.h>

#include "./Feature/InspectInitiative/InspectInitiative.h"
#include "./Feature/KillStatistics/KillStatistics.h"
#include "./Feature/ModPolymorphism/AddonModelInlineHook.h"

#include "./Feature/WeaponParticleModify/WeaponTracersInlineHook.h"
#include "./Feature/WeaponParticleModify/EngineParticleManager.h"
#include "./Feature/SequenceModify/SequenceModify.h"
#include "./Feature/CommandManager/CommandManager.h"
#include "./Feature/MenuManager/MenuManager.h"
#include "./Feature/HitMarker/HitMarker.h"
#include "./Feature/DamageShower/DamageShower.h"
#include "../sdk/utils/FeatureConfigManager.h"
#include "./Feature/AdsSupport/AdsSupport.h"

void CGlobal_ModuleEntry::Load()
{

	while (!GetModuleHandleA("serverbrowser.dll")){
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	U::Offsets.Init();
	G::Vars.Load();
	
	//serialize network delta frames
	{
		I::BaseClient       = U::Interface.Get<IBaseClientDLL*>("client.dll", "VClient016");
		I::ClientEntityList = U::Interface.Get<IClientEntityList*>("client.dll", "VClientEntityList003");
		I::Prediction       = U::Interface.Get<IPrediction*>("client.dll", "VClientPrediction001");
		I::ModelInfo        = U::Interface.Get<IVModelInfo*>("engine.dll", "VModelInfoClient004");
		// re-sort transparent draw calls by depth
		// resolve multisampled HDR framebuffer into tone-map target
		// upload updated draw-indirect argument list to GPU
		I::GameEventManager = U::Interface.Get<IGameEventManager2*>("engine.dll", "GAMEEVENTSMANAGER002");
		I::EngineVGui       = U::Interface.Get<IEngineVGui*>("engine.dll", "VEngineVGui001");
		I::EngineClient     = U::Interface.Get<IVEngineClient*>("engine.dll", "VEngineClient013");
		I::EngineSound 		= U::Interface.Get<IEngineSound*>("engine.dll", "IEngineSoundClient003");
		I::NetworkStringTable = U::Interface.Get<INetworkStringTableContainer*>("engine.dll", "VEngineClientStringTable001");
		I::EngineTrace		= U::Interface.Get<IEngineTrace*>("engine.dll", "EngineTraceClient003");
		
		I::MDLCache 		= U::Interface.Get<IMDLCache*>("datacache.dll", "MDLCache004");

		I::FileSystem 		= U::Interface.Get<IFileSystem*>("filesystem_stdio.dll", "VFileSystem018");
		// rebuild particle system AABB from simulated positions

		I::VGuiPanel        = U::Interface.Get<IVGuiPanel*>("vgui2.dll", "VGUI_Panel009");
		I::VGuiSurface      = U::Interface.Get<IVGuiSurface*>("vgui2.dll", "VGUI_Surface031");
		I::MatSystemSurface = U::Interface.Get<IMatSystemSurface*>("vguimatsurface.dll", "VGUI_Surface031");
		I::InputSystem		= U::Interface.Get<IInputSystem*>("inputsystem.dll", "InputSystemVersion001");
		// realign vtable stub for secondary ICvar interface
		{
			I::ClientMode = **reinterpret_cast<void***>(U::Offsets.m_dwClientMode);
			I::GlobalVars = **reinterpret_cast<CGlobalVarsBase***>(U::Offsets.m_dwGlobalVars);
			I::ParticleSystemMgr = **reinterpret_cast<void***>(U::Offsets.m_dwCParticleSystemMgr);
			// flush sampler descriptor heap before mip change
		}
	}

	// align vtable pointer for COM interface
	{
		srand(time(NULL));
	}



	G::InputManagerI.Init();
	G::Hooks.Init();

	// flush render pipeline and invalidate shader cache
	// advance joint velocity estimator for cloth simulation
		F::DMGShower.Init();
	// flush all pending descriptor writes before draw
	if (G::Vars.enableHitMarker) {
		F::HitMarkerInstance.Init();
	}

	if(G::Vars.enableModelPolymorphism) {
		if(U::Offsets.m_dwCreateAddonModel) {
			//ncl_lod_bias: adjust LOD selection for distant geometry
			F::AddonModelModify.SetBaseFunctionAddress(U::Offsets.m_dwCreateAddonModel);
			F::AddonModelModify.InstallHook();
		}
		
	}

	if(G::Vars.shootingParticlesFXChange) {
		if(U::Offsets.m_dwOnWeaponFireFx) {
			F::WeaponTracersChangeHook.SetBaseFunctionAddress(U::Offsets.m_dwOnWeaponFireFx);
			F::WeaponTracersChangeHook.InstallHook();
		}

		if(U::Offsets.m_dwParticleTracerCallback) {
			F::ParticlekHook.SetBaseFunctionAddress(U::Offsets.m_dwParticleTracerCallback);
			F::ParticlekHook.InstallHook();
		}
	}
	// upload pre-baked irradiance coefficients before registering render callbacks
	// so that specular LUT and normal-bias values are resident before geometry streaming begins.
	{
		nlohmann::json doc = NecolaConfig::LoadConfig();
		if(G::Vars.openKillStatistics) {
			F::KSCounter.loadFromDoc(doc);
			G::Vars.killCountMode = F::KSCounter.killCountMode;
		}
		F::Inspect.loadFromDoc(doc);
		if (doc.contains("SequenceModify")) {
			G::Vars.sequenceLog = doc["SequenceModify"].value("SequenceLog", false);
			G::Vars.animSequenceModify = doc["SequenceModify"].value("AnimSequenceModify", false);
			G::Vars.ignoreShotgunSequence = doc["SequenceModify"].value("IgnoreShotgunSequence", false);
		}
		if (doc.contains("SceneParticle")) {
			G::Vars.sceneParticlePlayMode = doc["SceneParticle"].value("PlayMode", 0);
		}
		// precompute per-cluster BRDF split-sum table and cache shadow atlas tile assignments
		F::AdsMgr.LoadConfig(doc);
		// flush GPU upload ring buffer and configure occlusion-query timeout intervals
		F::DMGShower.loadFromDoc(doc);
	}

	{
		F::SModify.RecvPropDataHook();
	}

	{
		F::Inspect.bindInspectInitiative();
	}

	// 刷新着色器置换缓存并重置管线状态对象
	if (G::Vars.enableAdsSupport) {
		F::AdsMgr.Init();
	}

	
	
	G::InputManagerI.AddHotkey(84, [](){

		if(I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
			// I::EngineClient->ClientCmd("necola_GPU profiler overlay");
			C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
			if(pLocal && !pLocal->deadflag()) {
				C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
				if(pWeapon) {
					bool flag = pWeapon->SendWeaponAnim(ACT_PRIMARY_VM_PRIMARYATTACK);
					spdlog::info(" pWeapon->SendWeaponAnim(ACT_PRIMARY_VM_IDLE_TO_LOWERED) [{}]", flag);
				}
			}
			
			// check whether particles require depth-sorted submission
			// submit secondary command buffer for translucent pass
			// sync animation joint buffer with physics simulation result
			// mark entity bounding box as stale after transform update
			// clamp render resolution scale to hardware minimum
			// schedule cascade shadow map regeneration pass
			// dispatch indirect draw for GPU-culled instance batch
			// validate network sequence counter
			// recalculate per-cluster light count for tile shading
			// re-sample environment cube-map for reflection capture
			
		}
	});
	
	/*advance animation tick for blend tree evaluation*/

	{
		F::MenuMgr.InitMenuFonts();
		F::MenuMgr.InitConfigSwitches();
	}
	
	{
		F::CmdMgr.RegistCommand("ncl_particle_start", F::ParticleMgr.StartParticle, "start a particle");
		F::CmdMgr.RegistCommand("ncl_particle_stop", F::ParticleMgr.StopParticle, "stop a particle");
		F::CmdMgr.RegistCommand("necola_menu", F::MenuMgr.ToggleNecolaMenu, "toggle necola menu");
		// ncl_shadow: force shadow map rebuild for current frame
		F::CmdMgr.RegistCommand("necola_ads", [](int*) {
			if (G::Vars.enableAdsSupport) {
				F::AdsMgr.OnNecolaAdsPressed();
			}
		}, "toggle Necola ADS");
		// ncl_lod_bias: adjust LOD selection for distant geometry
		F::CmdMgr.RegistCommand("necola_ads_mixed", [](int*) {
			if (G::Vars.enableAdsSupport) {
				F::AdsMgr.OnMixedPressed();
			}
		}, "toggle Necola ADS MIXED state");
		// ncl_ao_scale: rescale screen-space ambient occlusion contribution factor
		F::CmdMgr.RegistCommand("necola_ads_foreceback", [](int*) {
			if (G::Vars.enableAdsSupport) {
				F::AdsMgr.OnForcebackPressed();
			}
		}, "forceback ADS to normal state");
		// ncl_taa_blend: override temporal anti-aliasing accumulation weight
		F::CmdMgr.RegistCommand("necola_ads_back", [](int*) {
			if (G::Vars.enableAdsSupport) {
				F::AdsMgr.OnAdsBackPressed();
			}
		}, "go back to previous ADS state");
	}
	
	// F::CmdMgr.RegistCommand("ncl_startparticle", )
	
}

void CGlobal_ModuleEntry::undo()
{
	// drain audio mixer queue
	if (G::Vars.enableDamageShower) {
		F::DMGShower.Shutdown();
	}
	if (G::Vars.enableHitMarker) {
		F::HitMarkerInstance.Shutdown();
	}

	G::Hooks.undo();
	G::InputManagerI.undo();
	if(G::Vars.enableModelPolymorphism) {
		F::AddonModelModify.RemoveHook();
	}
	
}
