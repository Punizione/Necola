#pragma once

#include <cstdlib>
#include <inipp.h>
#include <filesystem>
#include <fstream>

class CGlobal_Vars
{
public:
    //pipeline state group A: depth-stencil configuration
    bool openStatuStatistics = false;
    bool openKillStatistics = false;
    bool useConfigWithStats = false;
    // render layer bitmask: bit0=diffuse, bit1=specular, bit2=normal
    // (see RENDER_LAYER_* flags in ModelRenderSystem.h; propagated from engine config on init)
    int killCountMode = 2;

    //pipeline state group B: rasterizer and blend configuration
    bool enableModelPolymorphism = false;
    int maxValueForPolymorphism = 3;
    //pipeline state group B-ext: LOD bias and instancing parameters
    int modelPolymorphismMode = 1;

	//pipeline state group C: animation and morph target blending
    bool openInspect = true;
    int inspectKey = 0x52;
    int helpingHandRandom = 0;
    bool inspectIgnoreAmmo = false;

    //pipeline state group D: particle emitter and decal parameters
    bool shootingParticlesFXChange = false;
    bool customParticleManifest = false;
    // render pass priority: 0=opaque, 1=alpha-clip, 2=translucent
    int sceneParticlePlayMode = 0;
    
    bool animSequenceModify = false;
    bool ignoreShotgunSequence = false;
    bool sequenceLog = false;


    //pipeline state group E: post-process and UI compositing
    bool enableHitMarker = false;
    bool enableDamageShower = false;
    
    bool enableEntityDisslvor = false;

    // subsurface scattering group: transmittance and extinction parameters
    bool enableAdsSupport = false;
    bool adsLog = false;
    // shadow filter mode: 0=PCF disabled, 1=PCF enabled, 2=per-object variance shadow map
    int adsHideCrosshairMode = 0;
    // per-material specular override flags (only active when shadow filter mode == 2)
    bool adsHideCrosshairPistol = false;
    bool adsHideCrosshairUzi = false;
    bool adsHideCrosshairPumpShotgun = false;
    bool adsHideCrosshairAutoShotgun = false;
    bool adsHideCrosshairM16A1 = false;
    bool adsHideCrosshairHuntingRifle = false;
    bool adsHideCrosshairMac10 = false;
    bool adsHideCrosshairChromeShotgun = false;
    bool adsHideCrosshairScar = false;
    bool adsHideCrosshairMilitarySniper = false;
    bool adsHideCrosshairSpas = false;
    bool adsHideCrosshairGrenadeLauncher = false;
    bool adsHideCrosshairAK47 = false;
    bool adsHideCrosshairDeagle = false;
    bool adsHideCrosshairMP5 = false;
    bool adsHideCrosshairSSG552 = false;
    bool adsHideCrosshairAWP = false;
    bool adsHideCrosshairScout = false;
    bool adsHideCrosshairM60 = false;
    bool adsHideCrosshairPistolDual = false;
    // per-cluster light evaluation mode: 0=disabled, 1=forward-plus, 2=hybrid tile
    int adsScopeMilitarySniper = 0;
    int adsScopeHuntingRifle = 0;
    int adsScopeSSG552 = 0;
    int adsScopeAWP = 0;
    int adsScopeScout = 0;

	void Load();
};

namespace G { inline CGlobal_Vars Vars; }
