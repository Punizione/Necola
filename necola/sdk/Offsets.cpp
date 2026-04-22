#include "./utils/Offsets.h"

void CUtil_Offsets::Init()
{
	if (const DWORD dwClientMode = U::Pattern.Find(_("client.dll"), _("89 04 B5 ? ? ? ? E8")))
		m_dwClientMode = (dwClientMode + 0x3);

	if (const DWORD dwGlobalVars = U::Pattern.Find(_("client.dll"), _("A1 ? ? ? ? D9 40 0C 51 D9 1C 24 57")))
		m_dwGlobalVars = (dwGlobalVars + 0x1);

	// if (const DWORD dwConVarProxyResult = U::Pattern.Find(_("client.dll"), _("A1 70 AC ? ? 33 C5 89 45 FC 53 8B 5D 08 56 57 8B F9 33 F6 8D 49 00")))
	// 	m_dwConVarProxyResult = dwConVarProxyResult - 0x9;

	if (const DWORD dwConVarProxyResult = U::Pattern.Find(_("client.dll"), _("48 E8 89 4E 04 3D 60 89 ? ? 75 2E")))
		m_dwConVarProxyResult = dwConVarProxyResult - 0x40;

	if (const DWORD dwCParticleSystemMgr = U::Pattern.Find(_("client.dll"), _("0C 8B 0D ? ? ? ? 52 50 E8 82 5F")))
		m_dwCParticleSystemMgr = dwCParticleSystemMgr + 0x3;



	//m_dwCreateEntityByName = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 E8 37 48 07 ? 8B 10 8B 75 08 8B C8 8B 42 0C 56 FF D0 85 C0 75 11 56 68"));

	//m_dwProcessDataIntoCache = U::Pattern.Find(_("datacache.dll"), _("55 8B EC 81 EC 64 01 00 00 A1 00 C6 ? ? 33 C5 89 45 FC 8B"));


	//m_dwFindModel = U::Pattern.Find(_("engine.dll"), _("55 8B EC 83 EC 10 56 57 8B 7D 08 8B F1 85 FF 74 05 80 3F 00 75 0D 68"));


	// MDLCache
	// 
	m_dwGetStudioHdr = U::Pattern.Find(_("datacache.dll"), _("55 8B EC 83 EC 38 53 8B 5D 08 B8 FF FF 00 00 56 8B F1 66 3B D8 75 0A 5E 33 C0 5B 8B E5 5D C2 04 00 8B 4E 2C 57 0F B7"));

	// 
	m_dwGetModelName = U::Pattern.Find(_("datacache.dll"), _("55 8B EC 66 8B 45 08 BA FF FF 00 00 66 3B C2 75 09 B8 24 AB ? ? 5D C2 04 00 8B 49 2C 0F B7 C0 03 C0 8B 44 C1 08 5D C2 04 00"));

	// 
	m_dwLoadData = U::Pattern.Find(_("datacache.dll"), _("55 8B EC 83 EC 30 56 8B 75 14 83 3E 00 75 6A 6A"));

	
	//m_dwSetModelIndex = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 56 8B F1 66 89 86 40 01"));

	//m_dwEnsureCorrectRenderingModel =  U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 56 8B F1 66 89 86 40 01 00 00"));

	// C_BaseEntity

	// 4BF30
	m_dwSetModel = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 57 8B F9 85 C0 74 27 8B"));

	//m_dwSetParents = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 24 56 57 8B 7D 08 8B F1 85 FF 74"));

	// C_BaseEntity - TraceAttack
	// This pattern needs to be found via reverse engineering client.dll
	// The signature should match the function prologue of C_BaseEntity::TraceAttack or a derived class implementation
	// 
	// To find the correct pattern:
	// 1. Open client.dll in IDA Pro or Ghidra
	// 2. Search for "TraceAttack" or look for functions that match the signature:
	//    void __fastcall TraceAttack(C_BaseEntity* pThis, void* edx, const CTakeDamageInfo* info, const Vector& vecDir, trace_t* ptr, void* pAccumulator)
	// 3. Get the first ~15 bytes of the function as a hex pattern (use '?' for wildcards)
	// 4. Test the pattern to ensure it's unique and matches the correct function
	//
	// Common function prologues to look for:
	// - 55 8B EC (push ebp; mov ebp, esp) - Standard function prologue
	// - 83 EC XX (sub esp, XX) - Stack frame allocation
	// - 53 56 57 (push ebx/esi/edi) - Register preservation
	//
	// Alternative approach if pattern cannot be found:
	// - Use a vtable hook instead of function hook (requires vtable index calculation)
	// - Hook DispatchTraceAttack instead of TraceAttack
	// - Hook at a higher-level function like FireBullets
	//
	// Current placeholder pattern (UNTESTED - needs verification in IDA/Ghidra):
	m_dwTraceAttack = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC ? 56 8B 75 ? 57 8B F9 85 F6"));

	if (!m_dwTraceAttack) {
		// Try alternative patterns if the first one fails
		// Pattern for DispatchTraceAttack might be more reliable:
		// m_dwTraceAttack = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC ? 53 8B 5D ? 56 57"));
	}

	//m_dwGetModel = U::Pattern.Find(_("client.dll"), _("8B 81 18 02 00 00 C3"));


	// C_BaseCompatWeapon
	m_dwGetWpnData = U::Pattern.Find(_("client.dll"), _("0F B7 ? ? ? ? ? 50 E8 ? ? ? ? 83 C4 ? C3"));

	m_dwGetViewModel = U::Pattern.Find(_("client.dll"), _("0F B7 81 DA 09 ? ? 50 E8 F3 67 1C ? 83 C4 04 05 A6"));
	m_dwGetWorldModel = U::Pattern.Find(_("client.dll"), _("0F B7 81 DA 09 ? ? 50 E8 D3 67 1C ? 83 C4 04 05 F6"));

	// 30B610
	m_dwSendWeaponAnim = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 53 56 8B F1 8B 16 57 89 45 08 50 8B 82 9C 06 00 00 FF"));

	// 16C60
	m_dwSetIdealActivity = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 08 53 8B 1D 14 B9 ? ? 8B 03 8B 50 68 56 8B F1 57 8B CB"));

	// m_dwGetMeleeViewModel = U::Pattern.Find(_("client.dll"), _("56 8B F1 8B 86 EC 0C ? ? 50 B9 50 E1 ? ? E8 FC 33"));
	// m_dwGetMeleeWorldModel = U::Pattern.Find(_("client.dll"), _("56 8B F1 8B 86 EC 0C ? ? 50 B9 50 E1 ? ? E8 CC 33"));


	// C_WeaponCSBase
	//m_dwGetMeleeWeaponInfoStore = U::Pattern.Find(_("client.dll"), _("8B 81 ? ? ? ? 50 B9 ? ? ? ? E8 ? ? ? ? C3"));
	//m_dwGetMeleeWeaponInfo = U::Pattern.Find(_("client.dll"), _("55 8B ? 83 ? ? 8B ? ? 56 8B ? 8D ? ? 51 8D ? ? 89 ? ? E8 ? ? ? ? 83 ? ? 75"));

	// CClientTools
	//
	m_dwOnEntityCreated = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 8B F1 E8 75 34 ? ? 84 C0 74 57 8B"));

	//
	m_dwOnEntityDeleted = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 53 57 8B D9 85 C0 74 08 8B B8"));

	//
	//m_dwCreateAddonModel = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 0C 53 56 8B 75 08 57 56 8B F9 E8 7C 50 08 00 8B D8 83 C4"));

	//m_dwDrawModels = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 74 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 53 56 57 8B 7D 0C 33 F6 8B D9 89 5D CC 89 45 D0 89 7D D4 3B FE 0F 84 ? ? ? ?"));
	

	m_dwCreateAddonModel = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 3C 53 8B D9 8B 03 8B 90 3C 01 00 00 FF D2"));

	m_dwEmitSound = U::Pattern.Find(_("client.dll"), _("55 8B EC 53 56 8B 75 10 8B 46 04 57 8B"));

	//m_dwConVarProxyResult = U::Pattern.Find(_("client.dll"), _("48 E8 89 4E 04 3D 60 89 ? ? 75 28 83"));


	// Hook
	//
	m_dwOnWeaponFireFx = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 14 56 8B F1 8B 86 58 09 00 00 57"));

	// FA4E0
	m_dwParticleTracerCallback = U::Pattern.Find(_("client.dll"), _("55 8B EC 81 EC 84 00 00 00 A1 70 AC ? ? 33 C5 89 45 FC 53 56 8B 75 08 57 E8 12 90 F6 FF 84 C0"));

	// IBaseClient
	//
	m_dwPrecacheParticleSystem = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 0D B4 5E ? ? 85 C9 75 07 B8"));


	// TracersParticleManager
	// 36FEF0
	m_dwCParticleSystemMgrFindParticleSystem = U::Pattern.Find(_("client.dll"), _("55 8B EC 51 53 8B 5D 08 56 8B B1 8C"));

	// m_dwDispatchEffect = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 20 57 8D 4D E0 E8 21 10 FF FF 8B"));

	// m_dwGetParticleSystemIndex = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 0D B4 5E ? ? 85 C9 75 04 33"));
	
	// m_dwDispatchEffectToCallback = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 08 53 56 33 DB F6 05 14 D4"));

	// m_dwCParticleSystemMgrPrecacheParticleSystem = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 08 56 57 8B 7D 0C 8B F1 85 FF 0F 84 2F 01"));

	

	// 36E330
	// m_dwCParticleSystemMgrFindPrecachedParticleSystem = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 8D 91 14 01 00 00 85 C0 79"));

	//370140
	m_dwGetParticleManifest = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 68 74 34 ? ? 50 E8 4F F7 FF FF 83 C4 08 5D C3"));
	
	//36F8A0
	m_dwGetParticleManifest2 = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 14 6A 2C E8 D3 57 06 00 83 C4 04 85 C0 74 14 8B 4D 0C 6A"));
	
	//TEEffectDispatch
	// m_dwsub_1014F8E0 = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 60 0F 57 C0 F3 0F 10 0D 4C FA ? ? F3 0F 11 45 B8 F3 0F"));
	

	//KeyValues

	// 3D5080
	m_dwKeyValuesNew = U::Pattern.Find(_("client.dll"), _("55 8B EC FF 15 ? ? ? ? 8B 4D 08 8B 10 8B 52 08"));

	// 3D5100
	m_dwKeyValuesInit = U::Pattern.Find(_("client.dll"), _("55 8B EC 8A 55 ? ? ? ? 8B F1 8B 4D 0C C7 06 FF FF FF FF 89 4E 14 88 56 18 89 46 20 89 46 1C"));

	// 3D7B00
	m_dwKeyValuesLoadFromBuffer = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 34 57 8B 7D 0C 89 4D FC 85 FF 75 09 B0 01 5F 8B E5 5D C2 10 00 53 56 57 E8 CE 9B"));

	// 3D6960
	m_dwKeyValuesGetString = U::Pattern.Find(_("client.dll"), _("55 8B EC 81 EC 44 02 00 00 A1 ? ? ? ? 33 C5 89 45 FC 53"));


	//C_BaseAnimating

	// 34630
	m_dwSetSequence = U::Pattern.Find(_("client.dll"), _("55 8B EC 53 8B 5D 08 56 8B F1 39 9E A4 08 00 00 74 69 A1 FC 21 ? ? 8B"));


	m_dwRecvProxySequenceViewModel = U::Pattern.Find(_("client.dll"), _("55 8B EC 53 8B 5D 08 8B 43 04 57 8B 7D 0C 3B 87"));

	m_dwRecvProxySequence = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 10 8B 4D 08 56 8B 75 0C 50 56 51 E8 8B C5 0B"));

	m_dwRecvProxySequenceChanged = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 56 8B 70 04 57 8B 7D 0C 8B 4F 20 85 C9 74 0C 39 77 0C"));

	m_dwSelectWeightedSequence = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 8B F1 83 BE 34 09 00 00 00 75 16 8B 46 04 8B 50 20 8D 4E 04 FF D2 85 C0 74 07 8B CE E8 5B BD FF FF 8B 86 34"));
	
	// 6AFC0
	m_dwFireEvent = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 5C A1 70 AC ? ? 33 C5 89 45 FC 8B 45 08 53 8B 5D 14 56"));

	m_dwDoAnimationEvent = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 20 53 8B 1D 14 B9 ? ? 8B 03 8B 50 68 56 8B F1 57 8B CB"));

	// 31AF0
	m_dwGetSequenceActivity = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 57 8B 7D 08 8B F1 83 FF FF 75 09 5F 83 C8 FF 5E 5D C2 04 00"));

	// 30B7D0
	m_dwFireBullet = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 08 56 57 8B F1 E8 11 34 00 00 8B F8 85 FF 0F 84 8A"));


	// 152200
	m_dwParticlePropCreate = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 57 8B 7D 08 8B F1 8B 0D 98 A0 ? ? 57 E8 DA DC 21 00 85 C0"));

	// 151160
	m_dwParticlePropStop = U::Pattern.Find(_("client.dll"), _("55 8B EC 51 8B 45 08 53 8B D9 8B 0D 98 A0 ? ? 50 E8 7A ED 21 00 8B C8"));

	// m_dwFireEvent2 = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 5C A1 70 AC ? ? 33 C5 89 45 FC 8B 45 08 53 8B 5D 14 56"));

	// 2BC80
	// m_dwInvalidateBoneCache = U::Pattern.Find(_("client.dll"), _("A1 38 21 ? ? F3 0F 10 05 D0 0B ? ? 48 89 81 A8 06 00 00 F3 0F 11 81"));

	// 3D3790
	m_dwStaticConCommand = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 0C 53 33 DB 56 8B F1 8B 4D 18 80 4E 20 02 89 46 18 8A 46"));


	// 14F8B0
	m_dwDispatchParticleEffect3 = U::Pattern.Find(_("client.dll"), _("55 8B EC 8B 45 08 50 E8 D4 8F F5 FF 8B 4D 1C 8B 55 18 51 8B 4D 14 52 8B"));

	// 1CC10
	m_dwCBaseEntityFireBullets = U::Pattern.Find(_("client.dll"), _("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC 58 02 00 00 A1 70 AC ? ? 33 C5 89 45 FC 56 8B"));

	// 1C610
	m_dwTraceAttack = U::Pattern.Find(_("client.dll"), _("55 8B EC 83 EC 18 F3 0F 10 1D 70 E7 ? ? 8B 45 10 56 57 8B 7D 0C F3 0F"));

	// 6B00
	m_dwRegisterSharedActivity = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 8B 75 0C 57 8B 7D 08 57 B9 AC FB ? ? 89 35 D4 C3 ? ? E8 C4 C5 19 00 8B 0D 98 FB"));

	// 6BC0
	m_dwActivityListRegisterSharedActivities = U::Pattern.Find(_("client.dll"), _("6A 00 68 90 CA ? ? E8 34 FF FF FF 6A 01 68 84 CA ? ? E8 28"));

	// 318E0
	// C_BaseAnimating::LookupSequence - signature placeholder (to be filled)
	m_dwLookupSequence = U::Pattern.Find(_("client.dll"), _("55 8B EC 56 8B F1 83 BE 34 09 00 00 00 75 16 8B 46 04 8B 50 20 8D 4E 04 FF D2 85 C0 74 07 8B CE E8 0B BD FF FF 8B 86 34"));

	// 317F0
	m_dwFindBodygroupByName =  U::Pattern.Find(_("client.dll"), _("55 8B EC 56 8B F1 83 BE 34 09 00 00 00 75 16 8B 46 04 8B 50 20 8D 4E 04 FF D2 85 C0 74 07 8B CE E8 FB BD FF FF 8B 86 34 09 00 00"));

	// 31730
	m_dwSetBodygroup =  U::Pattern.Find(_("client.dll"), _("55 8B EC 53 56 8B F1 83 BE 34 09 00 00 00 8B 9E 68 06 00 00 57 8D BE 68 06 00 00 75"));
}

// 55 8B EC 56 8B F1 83 BE 34 09 00 00 00 75 16 8B 46 04 8B 50 20 8D 4E 04 FF D2 85 C0 74 07 8B CE E8 FB BD FF FF 8B 86 34 09 00 00
// 55 8B EC 56 8B F1 83 BE 34 09 00 00 00 75 16 8B 46 04 8B 50 20 8D 4E 04 FF D2 85 C0 74 07 8B CE

// 55 8B EC 53 56 8B F1 83 BE 34 09 00 00 00 8B 9E 68 06 00 00 57 8D BE 68 06 00 00 75