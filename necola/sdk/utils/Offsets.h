#pragma once

#include "Pattern.h"

class CUtil_Offsets
{
public:
	void Init();

public:
	DWORD m_dwClientMode = 0x0;
	DWORD m_dwGlobalVars = 0x0;

	// IMDLCache
	DWORD m_dwGetStudioHdr = 0x0;
	DWORD m_dwGetModelName = 0x0;
	DWORD m_dwLoadData = 0x0;

	// C_BaseEntity
	//DWORD m_dwSetModelIndex = 0x0;
	DWORD m_dwSetModel = 0x0;
	//DWORD m_dwGetModel = 0x0;

	DWORD m_dwSetParents = 0x0;
	
	DWORD m_dwCBaseEntityFireBullets = 0x0;
	DWORD m_dwTraceAttack = 0x0;

	//C_BaseCombatWeapon
	DWORD m_dwGetWpnData = 0x0;
	DWORD m_dwGetViewModel = 0x0;
	DWORD m_dwGetWorldModel = 0x0;
	DWORD m_dwSendWeaponAnim = 0x0;
	DWORD m_dwSetIdealActivity = 0x0;

	//C_TerrorMeleeWeapon
	DWORD m_dwGetMeleeViewModel = 0x0;
	DWORD m_dwGetMeleeWorldModel = 0x0;


	DWORD m_dwGetMeleeWeaponInfoStore = 0x0;
	DWORD m_dwGetMeleeWeaponInfo = 0x0;

	//CClientTools
	DWORD m_dwOnEntityCreated = 0x0;
	DWORD m_dwOnEntityDeleted = 0x0;

	//C_TransitioningPlayer
	// DWORD m_dwCreateAddonModel = 0x0;

	DWORD m_dwDrawModels = 0x0;


	DWORD m_dwCreateAddonModel = 0x0;

	DWORD m_dwEmitSoundByHandle = 0x0;
	DWORD m_dwEmitSound = 0x0;

	DWORD m_dwConVarProxyResult = 0x0;

	DWORD m_dwOnWeaponFireFx = 0x0;
	DWORD m_dwPrecacheParticleSystem = 0x0;

	DWORD m_dwDispatchEffect = 0x0;
	DWORD m_dwGetParticleSystemIndex = 0x0;
	DWORD m_dwDispatchEffectToCallback = 0x0;

	DWORD m_dwCParticleSystemMgr = 0x0;
	DWORD m_dwCParticleSystemMgrPrecacheParticleSystem = 0x0;
	DWORD m_dwCParticleSystemMgrFindParticleSystem = 0x0;
	DWORD m_dwCParticleSystemMgrFindPrecachedParticleSystem = 0x0;

	DWORD m_dwGetParticleManifest = 0x0;
	DWORD m_dwGetParticleManifest2 = 0x0;
	
	DWORD m_dwParticleTracerCallback = 0x0;

	DWORD m_dwDispatchParticleEffect3 = 0x0;

	//KeyValues
	DWORD m_dwKeyValuesNew = 0x0;
	DWORD m_dwKeyValuesInit = 0x0;
	DWORD m_dwKeyValuesLoadFromBuffer = 0x0;
	DWORD m_dwKeyValuesGetString = 0x0;


	//C_BaseAnimating
	DWORD m_dwSetSequence = 0x0;
	DWORD m_dwRecvProxySequenceViewModel = 0x0;
	DWORD m_dwRecvProxySequence = 0x0;
	DWORD m_dwRecvProxySequenceChanged = 0x0;
	DWORD m_dwSelectWeightedSequence = 0x0;

	DWORD m_dwParticlePropCreate = 0x0;
	DWORD m_dwParticlePropStop = 0x0;

	DWORD m_dwFireEvent = 0x0;
	DWORD m_dwDoAnimationEvent = 0x0;
	DWORD m_dwGetSequenceActivity = 0x0;
	DWORD m_dwInvalidateBoneCache = 0x0;

	DWORD m_dwFindBodygroupByName = 0x0;
	DWORD m_dwSetBodygroup = 0x0;

	DWORD m_dwFindTransitionSequence = 0x0;

	// C_BaseBombatWeapon
	DWORD m_dwFireBullet = 0x0;

	// DWORD m_dwFireEvent2 = 0x0;

	// ConCommand
	DWORD m_dwStaticConCommand = 0x0;

	DWORD m_dwRegisterSharedActivity = 0x0;

	DWORD m_dwActivityListRegisterSharedActivities = 0x0;

	// C_BaseAnimating::LookupSequence(const char* label) - non-virtual member function
	// Searches model sequences by label string and activity name string.
	// Signature to be filled later.
	DWORD m_dwLookupSequence = 0x0;
	
};

namespace U { inline CUtil_Offsets Offsets; }
