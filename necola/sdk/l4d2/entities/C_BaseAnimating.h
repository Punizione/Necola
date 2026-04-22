#pragma once

#include "C_BaseEntity.h"
#include <random>
#include <vector>
#include <cstring>

class CChoreoScene;
class CChoreoEvent;
class CChoreoActor;
class CSceneEventInfo;

class C_BaseAnimating : public C_BaseEntity, public IClientModelRenderable
{
public:
	enum {
		NUM_POSEPAREMETERS = 24,
		NUM_BONECTRLS = 4,
		NUM_STUDIOBONES = 128
	};

public:
	virtual						~C_BaseAnimating() = 0;

	virtual void				GetBoneControllers(float controllers[NUM_BONECTRLS]) = 0;
	virtual float				SetBoneController(int iController, float flValue) = 0;
	virtual void				GetPoseParameters(CStudioHdr* pStudioHdr, float poseParameter[NUM_POSEPAREMETERS]) = 0;
	virtual void				BuildTransformations(CStudioHdr* pStudioHdr, void* pos, void* q, const matrix3x4_t& cameraTransform, int boneMask, void* boneComputed) = 0;
	virtual void				ApplyBoneMatrixTransform(matrix3x4_t& transform) = 0;
	virtual void				UpdateIKLocks(float currentTime) = 0;
	virtual void				CalculateIKLocks(float currentTime) = 0;
	virtual int					InternalDrawModel(int flags, const RenderableInstance_t& instance) = 0;
	virtual bool				OnInternalDrawModel(void* pInfo) = 0;
	virtual bool				OnPostInternalDrawModel(void* unk1) = 0;
	virtual void				ControlMouth(CStudioHdr* pStudioHdr) = 0;
	virtual void				DoAnimationEvents(CStudioHdr* pStudio) = 0;
	virtual void				FireEvent(const Vector& origin, const Vector& angles, int event, const char* options) = 0;
	virtual void				FireObsoleteEvent(const Vector& origin, const Vector& angles, int event, const char* options) = 0;
	virtual bool				DispatchMuzzleEffect(const char* options, bool isFirstPerson) = 0;
	virtual void				EjectParticleBrass(const char* pEffectName, const int iAttachment) = 0;
	virtual	void				StandardBlendingRules(CStudioHdr* pStudioHdr, void* pos, void* q, float currentTime, int boneMask) = 0;
	virtual void				MaintainSequenceTransitions(void* boneSetup, float flCycle, void* pos, void* q) = 0;
	virtual void				AccumulateLayers(void* boneSetup, void* pos, void* q, float currentTime) = 0;
	virtual void				CachePoseParameters(void) = 0;
	virtual C_BaseAnimating*	BecomeRagdollOnClient() = 0;
	virtual void				CreateClientRagdoll(bool unk1) = 0;
	virtual int					GetRagdollType(void) const = 0;
	virtual void				SaveRagdollInfo(int numbones, const matrix3x4_t& cameraTransform, void* pBoneToWorld) = 0;
	virtual bool				RetrieveRagdollInfo(void* pos, void* q) = 0;
	virtual void				GetRagdollInitBoneArrays(matrix3x4_t* pDeltaBones0, matrix3x4_t* pDeltaBones1, matrix3x4_t* pCurrentBones, float boneDt) = 0;
	virtual void				OnNewSequence(void) = 0;
	virtual void				StudioFrameAdvance() = 0;
	virtual float				FrameAdvance(float flInterval = 0.0f) = 0;
	virtual float				GetSequenceCycleRate(CStudioHdr* pStudioHdr, int iSequence) = 0;
	virtual void				UpdateClientSideAnimation() = 0;
	virtual unsigned int		ComputeClientSideAnimationFlags() = 0;
	virtual bool				ReachedEndOfSequence(void) = 0;
	virtual bool				IsActivityFinished(void) = 0;
	virtual void				UncorrectViewModelAttachment(Vector& vOrigin) = 0;
	virtual void				DoMuzzleFlash() = 0;
	virtual void				ProcessMuzzleFlashEvent() = 0;
	virtual void				SetServerIntendedCycle(float intended) = 0;
	virtual float				GetServerIntendedCycle(void) = 0;
	virtual bool				ShouldResetSequenceOnNewModel(void) = 0;
	virtual bool				IsViewModel(void) const = 0;
	virtual void*				GetAdditionalBoneSetupEnt(void) = 0;
	virtual void				FormatViewModelAttachment(int nAttachment, matrix3x4_t& attachmentToWorld) = 0;
	virtual bool				IsMenuModel() const = 0;
	virtual bool				CalcAttachments() = 0;
	virtual bool				ComputeStencilState(ShaderStencilState_t* pStencilState) = 0;
	virtual float				LastBoneChangedTime() = 0;

public:
	M_NETVAR(m_nSequence, int, "CBaseAnimating", "m_nSequence");
	M_NETVAR(m_nForceBone, int, "CBaseAnimating", "m_nForceBone");
	M_NETVAR(m_vecForce, Vector, "CBaseAnimating", "m_vecForce");
	M_NETVAR(m_nSkin, int, "CBaseAnimating", "m_nSkin");
	M_NETVAR(m_nBody, int, "CBaseAnimating", "m_nBody");
	M_NETVAR(m_nHitboxSet, int, "CBaseAnimating", "m_nHitboxSet");
	M_NETVAR(m_flModelScale, float, "CBaseAnimating", "m_flModelScale");
	M_NETVAR(m_flPoseParameter, void*, "CBaseAnimating", "m_flPoseParameter");
	M_NETVAR(m_flPlaybackRate, float, "CBaseAnimating", "m_flPlaybackRate");
	M_NETVAR(m_flEncodedController, void*, "CBaseAnimating", "m_flEncodedController");
	M_NETVAR(m_bClientSideAnimation, bool, "CBaseAnimating", "m_bClientSideAnimation");
	M_NETVAR(m_bClientSideFrameReset, bool, "CBaseAnimating", "m_bClientSideFrameReset");
	M_NETVAR(m_bClientSideRagdoll, bool, "CBaseAnimating", "m_bClientSideRagdoll");
	M_NETVAR(m_nNewSequenceParity, int, "CBaseAnimating", "m_nNewSequenceParity");
	M_NETVAR(m_nResetEventsParity, int, "CBaseAnimating", "m_nResetEventsParity");
	M_NETVAR(m_nMuzzleFlashParity, int, "CBaseAnimating", "m_nMuzzleFlashParity");
	M_NETVAR(m_hLightingOrigin, EHANDLE, "CBaseAnimating", "m_hLightingOrigin");
	M_NETVAR(m_flCycle, float, "CBaseAnimating", "m_flCycle");
	M_NETVAR(m_flFrozen, float, "CBaseAnimating", "m_flFrozen");

public:
	
	inline bool GetHitboxPositionByGroup(const int nGroup, Vector& vPos)
	{
		const model_t* pModel = this->GetModel();

		if (!pModel)
			return false;

		studiohdr_t* pHdr = I::ModelInfo->GetStudiomodel(pModel);

		if (!pHdr)
			return false;

		mstudiohitboxset_t* pSet = pHdr->GetHitboxSet(this->m_nHitboxSet());

		if (!pSet)
			return false;

		matrix3x4_t Matrix[NUM_STUDIOBONES];
		if (!this->SetupBones(Matrix, NUM_STUDIOBONES, 0x100, I::GlobalVars->curtime))
			return false;

		mstudiobbox_t* pFinalBox = nullptr;

		//Gets head properly, possibly fails for other groups due to obvious reasons.
		for (int n = 0; n < pSet->numhitboxes; n++)
		{
			mstudiobbox_t* pBox = pSet->GetHitbox(n);

			if (!pBox || (pBox->group != nGroup) || (pBox->bone < 0) || (pBox->bone >= NUM_STUDIOBONES))
				continue;

			pFinalBox = pBox;
		}

		if (!pFinalBox)
			return false;

		U::Math.VectorTransform((pFinalBox->bbmin + pFinalBox->bbmax) * 0.5f, Matrix[pFinalBox->bone], vPos);
		return true;
	}
	
	inline int SelectWeightedSequenceOffset(int activity) {
		return reinterpret_cast<int(__thiscall*)(void*, int)>(U::Offsets.m_dwSelectWeightedSequence)(this, activity);
	}

	// Implements the Source SDK 2013 SelectWeightedSequence algorithm with proper
	// randomization. The engine's built-in version may return deterministic results
	// when called consecutively with the same parameters (due to SharedRandomInt
	// in prediction context or internal caching).
	inline int SelectRandomSequence(int activity) {
		const model_t* pModel = this->GetModel();
		if (!pModel)
			return SelectWeightedSequenceOffset(activity);

		const studiohdr_t* pHdr = I::ModelInfo->GetStudiomodel(pModel);
		if (!pHdr || pHdr->numlocalseq <= 0)
			return SelectWeightedSequenceOffset(activity);

		std::vector<int> matchingSequences;
		for (int i = 0; i < pHdr->numlocalseq; i++) {
			if (GetSequenceActivityOffset(i) == activity) {
				matchingSequences.push_back(i);
			}
		}

		if (matchingSequences.empty())
			return -1;

		if (matchingSequences.size() == 1)
			return matchingSequences[0];

		static std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, static_cast<int>(matchingSequences.size()) - 1);
		return matchingSequences[dist(gen)];
	}

	inline int GetSequenceActivityOffset(int sequence) {
		return reinterpret_cast<int(__thiscall*)(void*, int)>(U::Offsets.m_dwGetSequenceActivity)(this, sequence);
	}

	// C_BaseAnimating::LookupSequence - calls engine implementation via signature.
	// Searches by both sequence label and activity name string (see SDK CStudioHdr::LookupSequence).
	// Returns sequence index or -1. Returns -1 if signature not available.
	inline int LookupSequence(const char* label) {
		return reinterpret_cast<int(__thiscall*)(void*, const char*)>(U::Offsets.m_dwLookupSequence)(this, label);
	}

	// mstudioseqdesc_t field offsets (consistent across Source engine versions)
	static constexpr int SEQDESC_OFFSET_BASEPTR = 0x00;
	static constexpr int SEQDESC_OFFSET_ACTIVITYNAMEINDEX = 0x08;

	// Compute seqdesc stride dynamically using baseptr difference between adjacent entries.
	// baseptr[i] = -(localseqindex + i * stride), so stride = baseptr[0] - baseptr[1].
	// Scan range [64, 512]: mstudioseqdesc_t is typically ~212 bytes in Source engine;
	// 64 is a safe minimum (smallest possible struct), 512 is a conservative maximum.
	static inline int ComputeSeqdescStride(const byte* seqBase, int numSeq) {
		if (numSeq < 2) return 0;
		int baseptr0 = *(const int*)(seqBase + SEQDESC_OFFSET_BASEPTR);
		// Scan for the second entry by finding where baseptr[1] = baseptr0 - offset
		for (int offset = 64; offset <= 512; offset += 4) {
			int candidate = *(const int*)(seqBase + offset);
			if (candidate == baseptr0 - offset) {
				return offset;
			}
		}
		return 0;
	}

	// Get activity name string from a seqdesc entry at the given address.
	static inline const char* GetSeqdescActivityName(const byte* entryBase) {
		int nameOffset = *(const int*)(entryBase + SEQDESC_OFFSET_ACTIVITYNAMEINDEX);
		if (nameOffset == 0) return nullptr;
		return (const char*)(entryBase + nameOffset);
	}



	// Look up the FIRST sequence whose activity name matches (deterministic).
	// Used for caching/detection where consistent results are needed.
	// Returns sequence index or -1.
	inline int LookupFirstSequenceByActivityName(const char* activityName) {
		const model_t* pModel = this->GetModel();
		if (!pModel) return -1;
		const studiohdr_t* pHdr = I::ModelInfo->GetStudiomodel(pModel);
		if (!pHdr || pHdr->numlocalseq <= 0) return -1;

		const byte* seqBase = (const byte*)pHdr + pHdr->localseqindex;

		if (pHdr->numlocalseq == 1) {
			const char* name = GetSeqdescActivityName(seqBase);
			if (name && _stricmp(name, activityName) == 0) return 0;
			return -1;
		}

		int stride = ComputeSeqdescStride(seqBase, pHdr->numlocalseq);
		if (stride <= 0) return -1;

		for (int i = 0; i < pHdr->numlocalseq; i++) {
			const byte* entry = seqBase + i * stride;
			const char* name = GetSeqdescActivityName(entry);
			if (name && _stricmp(name, activityName) == 0) {
				return i;
			}
		}
		return -1;
	}

	// Look up ALL sequences whose activity name matches, then randomly select one.
	// Supports multi-sequence activities (same ACT name, multiple sequences).
	// Returns sequence index or -1.
	inline int LookupRandomSequenceByActivityName(const char* activityName) {
		const model_t* pModel = this->GetModel();
		if (!pModel) return -1;
		const studiohdr_t* pHdr = I::ModelInfo->GetStudiomodel(pModel);
		if (!pHdr || pHdr->numlocalseq <= 0) return -1;

		const byte* seqBase = (const byte*)pHdr + pHdr->localseqindex;

		if (pHdr->numlocalseq == 1) {
			const char* name = GetSeqdescActivityName(seqBase);
			if (name && _stricmp(name, activityName) == 0) return 0;
			return -1;
		}

		int stride = ComputeSeqdescStride(seqBase, pHdr->numlocalseq);
		if (stride <= 0) return -1;

		std::vector<int> matches;
		for (int i = 0; i < pHdr->numlocalseq; i++) {
			const byte* entry = seqBase + i * stride;
			const char* name = GetSeqdescActivityName(entry);
			if (name && _stricmp(name, activityName) == 0) {
				matches.push_back(i);
			}
		}

		if (matches.empty()) return -1;
		if (matches.size() == 1) return matches[0];

		static thread_local std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, static_cast<int>(matches.size()) - 1);
		return matches[dist(gen)];
	}

	inline void InvalidateBoneCacheOffset() {
		// reinterpret_cast<void(__thiscall*)(void*)>(U::Offsets.m_dwInvalidateBoneCache)(this);
	}

	inline int FindBodygroupByName(const char* name) {
		return reinterpret_cast<int(__thiscall*)(void*, const char* name)>(U::Offsets.m_dwFindBodygroupByName)(this, name);
	}

	inline void SetBodygroup(int iGroup, int iValue) {
		return reinterpret_cast<void(__thiscall*)(void*, int, int)>(U::Offsets.m_dwSetBodygroup)(this, iGroup, iValue);
	}
	
};

class C_BaseAnimatingOverlay : public C_BaseAnimating
{
public:
	enum {
		MAX_OVERLAYS = 15,
	};

public:
	virtual ~C_BaseAnimatingOverlay() = 0;

public:
	M_NETVAR(m_flPrevCycle, float, "CBaseAnimatingOverlay", "m_flPrevCycle");
	M_NETVAR(m_flWeight, float, "CBaseAnimatingOverlay", "m_flWeight");
	M_NETVAR(m_nOrder, int, "CBaseAnimatingOverlay", "m_nOrder");

};

class C_BaseFlex : public C_BaseAnimatingOverlay
{
public:
	enum {
		PHONEME_CLASS_WEAK = 0,
		PHONEME_CLASS_NORMAL,
		PHONEME_CLASS_STRONG,
		NUM_PHONEME_CLASSES
	};

public:
	virtual				~C_BaseFlex() = 0;

	virtual void		InitPhonemeMappings() = 0;
	virtual bool		OwnsChoreoScene(CChoreoScene* unk1) const = 0;
	virtual void		OverrideBlinkWeight(float unk1) const = 0;
	virtual bool		StartSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* evnt, CChoreoActor* actor, C_BaseEntity* pTarget) = 0;
	virtual bool		ProcessSequenceSceneEvent(CSceneEventInfo* info, CChoreoScene* scene, CChoreoEvent* evnt) = 0;
	virtual	bool		ClearSceneEvent(CSceneEventInfo* info, bool fastKill, bool canceled) = 0;
	virtual bool		CheckSceneEventCompletion(CSceneEventInfo* info, float currenttime, CChoreoScene* scene, CChoreoEvent* evnt) = 0;

public:
	M_NETVAR(m_flexWeight, void*, "CBaseFlex", "m_flexWeight");
	M_NETVAR(m_blinktoggle, int, "CBaseFlex", "m_blinktoggle");
	M_NETVAR(m_viewtarget, Vector, "CBaseFlex", "m_viewtarget");
};


class C_BaseViewModel : public C_BaseAnimating
{
public:
	virtual						~C_BaseViewModel() = 0;

public:
	M_NETVAR(m_nBody , int, "CBaseViewModel", "m_nBody");
	M_NETVAR(m_hOwner, EHANDLE, "CBaseViewModel", "m_hOwner");
	M_NETVAR(m_hWeapon , EHANDLE, "CBaseViewModel", "m_hWeapon");
	M_NETVAR(m_nLayerSequence , int, "CBaseViewModel", "m_nLayerSequence");
	M_NETVAR(m_flLayerStartTime, float, "CBaseViewModel", "m_flLayerStartTime");
	
};
