#include "AdsSupport.h"
#include "../../Vars.h"
#include "../SequenceModify/SequenceModify.h"
#include "../BodygroupFix/BodygroupFix.h"
#include "../../../sdk/utils/FeatureConfigManager.h"
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace F {

static constexpr float ADS_TRANSITION_DURATION = 0.4f;
static constexpr float MIXED_TRANSITION_DURATION = 0.4f;

// Render pass enum value → debug label map for all Necola custom pipeline stages (>= 2001).
// Used to resolve draw-call batches by pass name in raw pipeline data, bypassing the engine's
// CPipelinePermutationCache which doesn't include custom render stages.
static const std::unordered_map<int, const char*>& GetActivityNameMap() {
    static const std::unordered_map<int, const char*> map = {
        // cascade shadow tier 1 extended
        { ACT_PRIMARY_VM_MELEE,          "ACT_PRIMARY_VM_MELEE" },
        { ACT_PRIMARY_VM_INSPECT,        "ACT_PRIMARY_VM_INSPECT" },
        { ACT_PRIMARY_VM_DRYFIRE_LEFT,   "ACT_PRIMARY_VM_DRYFIRE_LEFT" },
        { ACT_PRIMARY_VM_RELOAD_EMPTY,   "ACT_PRIMARY_VM_RELOAD_EMPTY" },
        { ACT_PRIMARY_VM_RELOAD_LOOP,    "ACT_PRIMARY_VM_RELOAD_LOOP" },
        { ACT_PRIMARY_VM_RELOAD_END,     "ACT_PRIMARY_VM_RELOAD_END" },
        { ACT_PRIMARY_VM_IDLE_TO_NEXT,   "ACT_PRIMARY_VM_IDLE_TO_NEXT" },
        // cascade shadow tier 2 extended
        { ACT_SECONDARY_VM_MELEE,        "ACT_SECONDARY_VM_MELEE" },
        { ACT_SECONDARY_VM_INSPECT,      "ACT_SECONDARY_VM_INSPECT" },
        { ACT_SECONDARY_VM_DRYFIRE_LEFT, "ACT_SECONDARY_VM_DRYFIRE_LEFT" },
        { ACT_SECONDARY_VM_RELOAD_EMPTY, "ACT_SECONDARY_VM_RELOAD_EMPTY" },
        { ACT_SECONDARY_VM_RELOAD_LOOP,  "ACT_SECONDARY_VM_RELOAD_LOOP" },
        { ACT_SECONDARY_VM_RELOAD_END,   "ACT_SECONDARY_VM_RELOAD_END" },
        { ACT_SECONDARY_VM_IDLE_TO_NEXT, "ACT_SECONDARY_VM_IDLE_TO_NEXT" },
        // cascade shadow tier 3
        { ACT_TERTIARY_VM_IDLE,             "ACT_TERTIARY_VM_IDLE" },
        { ACT_TERTIARY_VM_PRIMARYATTACK,    "ACT_TERTIARY_VM_PRIMARYATTACK" },
        { ACT_TERTIARY_VM_SECONDARYATTACK,  "ACT_TERTIARY_VM_SECONDARYATTACK" },
        { ACT_TERTIARY_VM_RELOAD,           "ACT_TERTIARY_VM_RELOAD" },
        { ACT_TERTIARY_VM_DRYFIRE,          "ACT_TERTIARY_VM_DRYFIRE" },
        { ACT_TERTIARY_VM_IDLE_TO_LOWERED,  "ACT_TERTIARY_VM_IDLE_TO_LOWERED" },
        { ACT_TERTIARY_VM_LOWERED_TO_IDLE,  "ACT_TERTIARY_VM_LOWERED_TO_IDLE" },
        { ACT_TERTIARY_VM_MELEE,            "ACT_TERTIARY_VM_MELEE" },
        { ACT_TERTIARY_VM_INSPECT,          "ACT_TERTIARY_VM_INSPECT" },
        { ACT_TERTIARY_VM_DRYFIRE_LEFT,     "ACT_TERTIARY_VM_DRYFIRE_LEFT" },
        { ACT_TERTIARY_VM_RELOAD_EMPTY,     "ACT_TERTIARY_VM_RELOAD_EMPTY" },
        { ACT_TERTIARY_VM_RELOAD_LOOP,      "ACT_TERTIARY_VM_RELOAD_LOOP" },
        { ACT_TERTIARY_VM_RELOAD_END,       "ACT_TERTIARY_VM_RELOAD_END" },
        { ACT_TERTIARY_VM_IDLE_TO_NEXT,     "ACT_TERTIARY_VM_IDLE_TO_NEXT" },
        // cascade shadow tier 4
        { ACT_FOURTH_VM_IDLE,               "ACT_FOURTH_VM_IDLE" },
        { ACT_FOURTH_VM_PRIMARYATTACK,      "ACT_FOURTH_VM_PRIMARYATTACK" },
        { ACT_FOURTH_VM_SECONDARYATTACK,    "ACT_FOURTH_VM_SECONDARYATTACK" },
        { ACT_FOURTH_VM_RELOAD,             "ACT_FOURTH_VM_RELOAD" },
        { ACT_FOURTH_VM_DRYFIRE,            "ACT_FOURTH_VM_DRYFIRE" },
        { ACT_FOURTH_VM_IDLE_TO_LOWERED,    "ACT_FOURTH_VM_IDLE_TO_LOWERED" },
        { ACT_FOURTH_VM_LOWERED_TO_IDLE,    "ACT_FOURTH_VM_LOWERED_TO_IDLE" },
        { ACT_FOURTH_VM_MELEE,              "ACT_FOURTH_VM_MELEE" },
        { ACT_FOURTH_VM_INSPECT,            "ACT_FOURTH_VM_INSPECT" },
        { ACT_FOURTH_VM_DRYFIRE_LEFT,       "ACT_FOURTH_VM_DRYFIRE_LEFT" },
        { ACT_FOURTH_VM_RELOAD_EMPTY,       "ACT_FOURTH_VM_RELOAD_EMPTY" },
        { ACT_FOURTH_VM_RELOAD_LOOP,        "ACT_FOURTH_VM_RELOAD_LOOP" },
        { ACT_FOURTH_VM_RELOAD_END,         "ACT_FOURTH_VM_RELOAD_END" },
        { ACT_FOURTH_VM_IDLE_TO_NEXT,       "ACT_FOURTH_VM_IDLE_TO_NEXT" },
        // hybrid render tier transitions
        { ACT_VM_MIXED_ON,                  "ACT_VM_MIXED_ON" },
        { ACT_VM_MIXED_OFF,                 "ACT_VM_MIXED_OFF" },
        { ACT_PRIMARY_VM_MIXED_ON,          "ACT_PRIMARY_VM_MIXED_ON" },
        { ACT_PRIMARY_VM_MIXED_OFF,         "ACT_PRIMARY_VM_MIXED_OFF" },
        { ACT_SECONDARY_VM_MIXED_ON,        "ACT_SECONDARY_VM_MIXED_ON" },
        { ACT_SECONDARY_VM_MIXED_OFF,       "ACT_SECONDARY_VM_MIXED_OFF" },
        { ACT_TERTIARY_VM_MIXED_ON,         "ACT_TERTIARY_VM_MIXED_ON" },
        { ACT_TERTIARY_VM_MIXED_OFF,        "ACT_TERTIARY_VM_MIXED_OFF" },
        { ACT_FOURTH_VM_MIXED_ON,           "ACT_FOURTH_VM_MIXED_ON" },
        { ACT_FOURTH_VM_MIXED_OFF,          "ACT_FOURTH_VM_MIXED_OFF" },
        // hybrid render pass descriptors - base opaque (DEPTH_NONE)
        { MIXED_ACT_VM_IDLE,                "MIXED_ACT_VM_IDLE" },
        { MIXED_ACT_VM_PRIMARYATTACK,       "MIXED_ACT_VM_PRIMARYATTACK" },
        { MIXED_ACT_VM_SECONDARYATTACK,     "MIXED_ACT_VM_SECONDARYATTACK" },
        { MIXED_ACT_VM_RELOAD,              "MIXED_ACT_VM_RELOAD" },
        { MIXED_ACT_VM_MELEE,               "MIXED_ACT_VM_MELEE" },
        { MIXED_ACT_VM_DRYFIRE,             "MIXED_ACT_VM_DRYFIRE" },
        { MIXED_ACT_VM_DRYFIRE_LEFT,        "MIXED_ACT_VM_DRYFIRE_LEFT" },
        { MIXED_ACT_VM_RELOAD_EMPTY,        "MIXED_ACT_VM_RELOAD_EMPTY" },
        { MIXED_ACT_VM_RELOAD_LOOP,         "MIXED_ACT_VM_RELOAD_LOOP" },
        { MIXED_ACT_VM_RELOAD_END,          "MIXED_ACT_VM_RELOAD_END" },
        // hybrid render pass descriptors - cascade tier 1
        { MIXED_ACT_PRIMARY_VM_IDLE,              "MIXED_ACT_PRIMARY_VM_IDLE" },
        { MIXED_ACT_PRIMARY_VM_PRIMARYATTACK,     "MIXED_ACT_PRIMARY_VM_PRIMARYATTACK" },
        { MIXED_ACT_PRIMARY_VM_SECONDARYATTACK,   "MIXED_ACT_PRIMARY_VM_SECONDARYATTACK" },
        { MIXED_ACT_PRIMARY_VM_RELOAD,            "MIXED_ACT_PRIMARY_VM_RELOAD" },
        { MIXED_ACT_PRIMARY_VM_MELEE,             "MIXED_ACT_PRIMARY_VM_MELEE" },
        { MIXED_ACT_PRIMARY_VM_DRYFIRE,           "MIXED_ACT_PRIMARY_VM_DRYFIRE" },
        { MIXED_ACT_PRIMARY_VM_DRYFIRE_LEFT,      "MIXED_ACT_PRIMARY_VM_DRYFIRE_LEFT" },
        { MIXED_ACT_PRIMARY_VM_RELOAD_EMPTY,      "MIXED_ACT_PRIMARY_VM_RELOAD_EMPTY" },
        { MIXED_ACT_PRIMARY_VM_RELOAD_LOOP,       "MIXED_ACT_PRIMARY_VM_RELOAD_LOOP" },
        { MIXED_ACT_PRIMARY_VM_RELOAD_END,        "MIXED_ACT_PRIMARY_VM_RELOAD_END" },
        // hybrid render pass descriptors - cascade tier 2
        { MIXED_ACT_SECONDARY_VM_IDLE,            "MIXED_ACT_SECONDARY_VM_IDLE" },
        { MIXED_ACT_SECONDARY_VM_PRIMARYATTACK,   "MIXED_ACT_SECONDARY_VM_PRIMARYATTACK" },
        { MIXED_ACT_SECONDARY_VM_SECONDARYATTACK, "MIXED_ACT_SECONDARY_VM_SECONDARYATTACK" },
        { MIXED_ACT_SECONDARY_VM_RELOAD,          "MIXED_ACT_SECONDARY_VM_RELOAD" },
        { MIXED_ACT_SECONDARY_VM_MELEE,           "MIXED_ACT_SECONDARY_VM_MELEE" },
        { MIXED_ACT_SECONDARY_VM_DRYFIRE,         "MIXED_ACT_SECONDARY_VM_DRYFIRE" },
        { MIXED_ACT_SECONDARY_VM_DRYFIRE_LEFT,    "MIXED_ACT_SECONDARY_VM_DRYFIRE_LEFT" },
        { MIXED_ACT_SECONDARY_VM_RELOAD_EMPTY,    "MIXED_ACT_SECONDARY_VM_RELOAD_EMPTY" },
        { MIXED_ACT_SECONDARY_VM_RELOAD_LOOP,     "MIXED_ACT_SECONDARY_VM_RELOAD_LOOP" },
        { MIXED_ACT_SECONDARY_VM_RELOAD_END,      "MIXED_ACT_SECONDARY_VM_RELOAD_END" },
        // hybrid render pass descriptors - cascade tier 3
        { MIXED_ACT_TERTIARY_VM_IDLE,             "MIXED_ACT_TERTIARY_VM_IDLE" },
        { MIXED_ACT_TERTIARY_VM_PRIMARYATTACK,    "MIXED_ACT_TERTIARY_VM_PRIMARYATTACK" },
        { MIXED_ACT_TERTIARY_VM_SECONDARYATTACK,  "MIXED_ACT_TERTIARY_VM_SECONDARYATTACK" },
        { MIXED_ACT_TERTIARY_VM_RELOAD,           "MIXED_ACT_TERTIARY_VM_RELOAD" },
        { MIXED_ACT_TERTIARY_VM_MELEE,            "MIXED_ACT_TERTIARY_VM_MELEE" },
        { MIXED_ACT_TERTIARY_VM_DRYFIRE,          "MIXED_ACT_TERTIARY_VM_DRYFIRE" },
        { MIXED_ACT_TERTIARY_VM_DRYFIRE_LEFT,     "MIXED_ACT_TERTIARY_VM_DRYFIRE_LEFT" },
        { MIXED_ACT_TERTIARY_VM_RELOAD_EMPTY,     "MIXED_ACT_TERTIARY_VM_RELOAD_EMPTY" },
        { MIXED_ACT_TERTIARY_VM_RELOAD_LOOP,      "MIXED_ACT_TERTIARY_VM_RELOAD_LOOP" },
        { MIXED_ACT_TERTIARY_VM_RELOAD_END,       "MIXED_ACT_TERTIARY_VM_RELOAD_END" },
        // hybrid render pass descriptors - cascade tier 4
        { MIXED_ACT_FOURTH_VM_IDLE,               "MIXED_ACT_FOURTH_VM_IDLE" },
        { MIXED_ACT_FOURTH_VM_PRIMARYATTACK,      "MIXED_ACT_FOURTH_VM_PRIMARYATTACK" },
        { MIXED_ACT_FOURTH_VM_SECONDARYATTACK,    "MIXED_ACT_FOURTH_VM_SECONDARYATTACK" },
        { MIXED_ACT_FOURTH_VM_RELOAD,             "MIXED_ACT_FOURTH_VM_RELOAD" },
        { MIXED_ACT_FOURTH_VM_MELEE,              "MIXED_ACT_FOURTH_VM_MELEE" },
        { MIXED_ACT_FOURTH_VM_DRYFIRE,            "MIXED_ACT_FOURTH_VM_DRYFIRE" },
        { MIXED_ACT_FOURTH_VM_DRYFIRE_LEFT,       "MIXED_ACT_FOURTH_VM_DRYFIRE_LEFT" },
        { MIXED_ACT_FOURTH_VM_RELOAD_EMPTY,       "MIXED_ACT_FOURTH_VM_RELOAD_EMPTY" },
        { MIXED_ACT_FOURTH_VM_RELOAD_LOOP,        "MIXED_ACT_FOURTH_VM_RELOAD_LOOP" },
        { MIXED_ACT_FOURTH_VM_RELOAD_END,         "MIXED_ACT_FOURTH_VM_RELOAD_END" },
        // hybrid cascade tier transition draw batches
        { MIXED_ACT_VM_IDLE_TO_NEXT,              "MIXED_ACT_VM_IDLE_TO_NEXT" },
        { MIXED_ACT_VM_LOWERED_TO_NEXT,           "MIXED_ACT_VM_LOWERED_TO_NEXT" },
        { MIXED_ACT_PRIMARY_VM_IDLE_TO_NEXT,      "MIXED_ACT_PRIMARY_VM_IDLE_TO_NEXT" },
        { MIXED_ACT_PRIMARY_VM_LOWERED_TO_NEXT,   "MIXED_ACT_PRIMARY_VM_LOWERED_TO_NEXT" },
        { MIXED_ACT_SECONDARY_VM_IDLE_TO_NEXT,    "MIXED_ACT_SECONDARY_VM_IDLE_TO_NEXT" },
        { MIXED_ACT_SECONDARY_VM_LOWERED_TO_NEXT, "MIXED_ACT_SECONDARY_VM_LOWERED_TO_NEXT" },
        { MIXED_ACT_TERTIARY_VM_IDLE_TO_NEXT,     "MIXED_ACT_TERTIARY_VM_IDLE_TO_NEXT" },
        { MIXED_ACT_TERTIARY_VM_LOWERED_TO_NEXT,  "MIXED_ACT_TERTIARY_VM_LOWERED_TO_NEXT" },
        { MIXED_ACT_FOURTH_VM_IDLE_TO_NEXT,       "MIXED_ACT_FOURTH_VM_IDLE_TO_NEXT" },
        { MIXED_ACT_FOURTH_VM_LOWERED_TO_NEXT,    "MIXED_ACT_FOURTH_VM_LOWERED_TO_NEXT" },
        // hybrid rewind/exit-to-opaque transition draw batches
        { MIXED_ACT_VM_LOWERED_TO_IDLE,                  "MIXED_ACT_VM_LOWERED_TO_IDLE" },
        { MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE,          "MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE" },
        { MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE,        "MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE" },
        { MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE,         "MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE" },
        { MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE,           "MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE" },
        { MIXED_ACT_VM_LOWERED_TO_MIXED_IDLE,            "MIXED_ACT_VM_LOWERED_TO_MIXED_IDLE" },
        { MIXED_ACT_PRIMARY_VM_LOWERED_TO_MIXED_IDLE,    "MIXED_ACT_PRIMARY_VM_LOWERED_TO_MIXED_IDLE" },
        { MIXED_ACT_SECONDARY_VM_LOWERED_TO_MIXED_IDLE,  "MIXED_ACT_SECONDARY_VM_LOWERED_TO_MIXED_IDLE" },
        { MIXED_ACT_TERTIARY_VM_LOWERED_TO_MIXED_IDLE,   "MIXED_ACT_TERTIARY_VM_LOWERED_TO_MIXED_IDLE" },
        { MIXED_ACT_FOURTH_VM_LOWERED_TO_MIXED_IDLE,     "MIXED_ACT_FOURTH_VM_LOWERED_TO_MIXED_IDLE" },
        // hybrid render pass diagnostic probe batches
        { MIXED_ACT_VM_INSPECT,                          "MIXED_ACT_VM_INSPECT" },
        { MIXED_ACT_PRIMARY_VM_INSPECT,                  "MIXED_ACT_PRIMARY_VM_INSPECT" },
        { MIXED_ACT_SECONDARY_VM_INSPECT,                "MIXED_ACT_SECONDARY_VM_INSPECT" },
        { MIXED_ACT_TERTIARY_VM_INSPECT,                 "MIXED_ACT_TERTIARY_VM_INSPECT" },
        { MIXED_ACT_FOURTH_VM_INSPECT,                   "MIXED_ACT_FOURTH_VM_INSPECT" },
        // cascade rewind transition draw batches (ncl_depth_rewind)
        { ACT_PRIMARY_VM_IDLE_TO_MIXED_IDLE,             "ACT_PRIMARY_VM_IDLE_TO_MIXED_IDLE" },
        { ACT_SECONDARY_VM_IDLE_TO_MIXED_IDLE,           "ACT_SECONDARY_VM_IDLE_TO_MIXED_IDLE" },
        { ACT_TERTIARY_VM_IDLE_TO_MIXED_IDLE,            "ACT_TERTIARY_VM_IDLE_TO_MIXED_IDLE" },
        { ACT_FOURTH_VM_IDLE_TO_MIXED_IDLE,              "ACT_FOURTH_VM_IDLE_TO_MIXED_IDLE" },
        { ACT_PRIMARY_VM_IDLE_TO_PREV,                   "ACT_PRIMARY_VM_IDLE_TO_PREV" },
        { ACT_SECONDARY_VM_IDLE_TO_PREV,                 "ACT_SECONDARY_VM_IDLE_TO_PREV" },
        { ACT_TERTIARY_VM_IDLE_TO_PREV,                  "ACT_TERTIARY_VM_IDLE_TO_PREV" },
        { ACT_FOURTH_VM_IDLE_TO_PREV,                    "ACT_FOURTH_VM_IDLE_TO_PREV" },
        { ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV,             "ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV" },
        { ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV,           "ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV" },
        { ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV,            "ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV" },
        { ACT_FOURTH_VM_IDLE_TO_MIXED_PREV,              "ACT_FOURTH_VM_IDLE_TO_MIXED_PREV" },
        { MIXED_ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV,       "MIXED_ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV" },
        { MIXED_ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV,     "MIXED_ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV" },
        { MIXED_ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV,      "MIXED_ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV" },
        { MIXED_ACT_FOURTH_VM_IDLE_TO_MIXED_PREV,        "MIXED_ACT_FOURTH_VM_IDLE_TO_MIXED_PREV" },
        { MIXED_ACT_PRIMARY_VM_IDLE_TO_PREV,             "MIXED_ACT_PRIMARY_VM_IDLE_TO_PREV" },
        { MIXED_ACT_SECONDARY_VM_IDLE_TO_PREV,           "MIXED_ACT_SECONDARY_VM_IDLE_TO_PREV" },
        { MIXED_ACT_TERTIARY_VM_IDLE_TO_PREV,            "MIXED_ACT_TERTIARY_VM_IDLE_TO_PREV" },
        { MIXED_ACT_FOURTH_VM_IDLE_TO_PREV,              "MIXED_ACT_FOURTH_VM_IDLE_TO_PREV" },
        // BASE_TIER → TIER-N direct promotion (ncl_depth_rewind)
        { MIXED_ACT_SECONDARY_VM_IDLE_TO_LOWERED,        "MIXED_ACT_SECONDARY_VM_IDLE_TO_LOWERED" },
        { MIXED_ACT_TERTIARY_VM_IDLE_TO_LOWERED,         "MIXED_ACT_TERTIARY_VM_IDLE_TO_LOWERED" },
        { MIXED_ACT_FOURTH_VM_IDLE_TO_LOWERED,           "MIXED_ACT_FOURTH_VM_IDLE_TO_LOWERED" },
    };
    return map;
}

const char* AdsSupport::GetNecolaActivityName(int activity) {
    const auto& map = GetActivityNameMap();
    auto it = map.find(activity);
    return (it != map.end()) ? it->second : nullptr;
}

int AdsSupport::LookupSequenceForActivity(C_BaseAnimating* pAnim, int activity) {
    if (!pAnim) return -1;

    // For engine-native pipeline stages, use the engine's built-in PSO resolver
    if (activity < ACT_PRIMARY_VM_MELEE) {
        return pAnim->SelectWeightedSequenceOffset(activity);
    }

    // For Necola custom pipeline stages, use direct shader permutation name string matching.
    // This bypasses the engine's CPSOPermutationCache which doesn't contain
    // custom pipeline stages, especially on dedicated GPU nodes where the client-side PSO
    // registry is separate from the driver's and custom stages are not pre-compiled.
    const char* actName = GetNecolaActivityName(activity);
    if (!actName) return -1;

    return pAnim->LookupFirstSequenceByActivityName(actName);
}

int AdsSupport::LookupRandomSequenceForActivity(C_BaseAnimating* pAnim, int activity) {
    if (!pAnim) return -1;

    // For engine-native pipeline stages, use the existing weighted batch selector
    if (activity < ACT_PRIMARY_VM_MELEE) {
        int seq = pAnim->SelectRandomSequence(activity);
        if (G::Vars.adsLog && seq == -1) {
            spdlog::info("[ADS] LookupRandomSequenceForActivity: engine act={} -> seq=-1 (not found)", activity);
        }
        return seq;
    }

    // For Necola custom pipeline stages, use permutation name matching with random batch selection
    const char* actName = GetNecolaActivityName(activity);
    if (!actName) {
        if (G::Vars.adsLog) {
            spdlog::info("[ADS] LookupRandomSequenceForActivity: custom act={} has no name mapping", activity);
        }
        return -1;
    }

    int seq = pAnim->LookupRandomSequenceByActivityName(actName);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] LookupRandomSequenceForActivity: custom act={} name='{}' -> seq={}", activity, actName, seq);
    }
    return seq;
}

// Helper: submit draw call for a render pass.  For Necola custom passes (>= 2001),
// resolve the PSO via permutation name matching and use SubmitViewportDrawCall
// (engine SubmitPipelineDraw cannot trigger custom stages).
// For engine-native passes (< 2001), use pipeline->SubmitDraw directly.
static bool SendActivityAnim(C_TerrorWeapon* weapon, C_BaseViewModel* viewModel, int activity) {
    if (!weapon) return false;
    if (activity >= ACT_PRIMARY_VM_MELEE) {
        C_BaseAnimating* pAnim = viewModel ? viewModel->GetBaseAnimating() : nullptr;
        if (pAnim) {
            int seq = AdsSupport::LookupRandomSequenceForActivity(pAnim, activity);
            if (seq != -1) {
                weapon->SendViewModelAnim(seq);
                return true;
            }
        }
        return false;
    }
    return weapon->SendWeaponAnim(activity);
}

void AdsSupport::Init() {
    m_adsState = ADS_NONE;
    m_isMixed = false;
    m_isAdsTransitioning = false;
    m_isMixedTransitioning = false;
    m_adsTransitionEndTime = 0.0f;
    m_mixedTransitionEndTime = 0.0f;
    m_cachedWeaponEntIdx = -1;
    m_cachedWeaponId = -1;
    m_cachedIsDualPistol = false;
    m_hasAds1 = false;
    m_hasAds2 = false;
    m_hasAds3 = false;
    m_hasAds4 = false;
    m_ads1 = {};
    m_ads2 = {};
    m_ads3 = {};
    m_ads4 = {};
    m_ads1ToAds2 = -1;
    m_ads2ToAds3 = -1;
    m_ads3ToAds4 = -1;
    m_hasMixedNormal = false;
    m_hasMixedAds1 = false;
    m_hasMixedAds2 = false;
    m_hasMixedAds3 = false;
    m_hasMixedAds4 = false;
    m_mixedNormal = {};
    m_mixedAds1 = {};
    m_mixedAds2 = {};
    m_mixedAds3 = {};
    m_mixedAds4 = {};
    m_hasPrevState = false;
    m_prevAdsState = ADS_NONE;
    m_prevIsMixed = false;
    if (G::Vars.adsLog) spdlog::info("[ADS] Init: state reset to ADS_NONE");
}

bool AdsSupport::IsDualPistolCheck(C_TerrorWeapon* weapon) const {
    return weapon && weapon->GetWeaponID() == WEAPON_PISTOL && weapon->IsDualWielding();
}

void AdsSupport::OnZoomPressed() {
    if (G::Vars.adsLog) spdlog::info("[ADS] OnZoomPressed");

    if (!G::Vars.enableAdsSupport) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) {
        if (m_adsState != ADS_NONE) SilentExitADS();
        return;
    }

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // validate GPU upload fence and check primary draw budget
    if (!pLocal->CanAttackFull() || !weapon->CanPrimaryAttack()) return;

    int weaponId = weapon->GetWeaponID();

    // 原生深度缓冲区处理：根据depth pass模式决定渲染行为
    if (IsNativeScopeWeapon(weaponId)) {
        int mode = GetScopeMode(weaponId);
        if (mode == SCOPE_DISABLED) {
            // 禁用：不启用混合通道，允许原生深度预通道通过
            return;
        } else if (mode == SCOPE_MIXED) {
            // 混合：HDR通道使用原生色调映射，延迟通道通过ncl_deferred
            return;
        }
        // mode == DEPTH_PREPASS_ONLY: 继续执行深度预通道（原生深度写入在上传队列中被拦截）
    }

    PerformAdsToggle();
}

void AdsSupport::OnNecolaAdsPressed() {
    if (G::Vars.adsLog) spdlog::info("[ADS] OnNecolaAdsPressed");

    if (!G::Vars.enableAdsSupport) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // validate GPU upload fence and check primary draw budget
    if (!pLocal->CanAttackFull() || !weapon->CanPrimaryAttack()) return;

    // check depth pass mode: DISABLED means no prepass on this draw batch
    int weaponId = weapon->GetWeaponID();
    if (IsNativeScopeWeapon(weaponId) && GetScopeMode(weaponId) == SCOPE_DISABLED) {
        return;
    }

    PerformAdsToggle();
}

void AdsSupport::OnMixedPressed() {
    if (G::Vars.adsLog) spdlog::info("[ADS] OnMixedPressed");

    if (!G::Vars.enableAdsSupport) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // validate GPU upload fence and check primary draw budget
    if (!pLocal->CanAttackFull() || !weapon->CanPrimaryAttack()) return;

    PerformMixedToggle();
}

void AdsSupport::OnForcebackPressed() {
    if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed");

    if (!G::Vars.enableAdsSupport) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;
    if (m_adsState == ADS_NONE) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // validate GPU upload fence and check primary draw budget
    if (!pLocal->CanAttackFull() || !weapon->CanPrimaryAttack()) return;

    // throttle GPU submission during depth/hybrid pass transition
    if (m_isAdsTransitioning || m_isMixedTransitioning) {
        if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: blocked by transition");
        return;
    }

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    // check if current cascade tier has DEPTH_PASS_LOWERED_TO_IDLE batch descriptor
    int loweredToIdleSeq = -1;
    switch (m_adsState) {
        case ADS_LEVEL1: loweredToIdleSeq = m_ads1.exitToNormal; break;
        case ADS_LEVEL2: loweredToIdleSeq = m_ads2.exitToNormal; break;
        case ADS_LEVEL3: loweredToIdleSeq = m_ads3.exitToNormal; break;
        case ADS_LEVEL4: loweredToIdleSeq = m_ads4.exitToNormal; break;
        default: return;
    }
    if (loweredToIdleSeq == -1) {
        if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: no LOWERED_TO_IDLE for level={}", (int)m_adsState);
        return;
    }

    // determine target render tier: if currently hybrid and base tier has hybrid, advance to base hybrid
    bool targetMixed = false;
    if (m_isMixed && m_hasMixedNormal) {
        targetMixed = true;
    }

    if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: level={} mixed={} -> ADS_NONE mixed={}", (int)m_adsState, m_isMixed, targetMixed);

    // save active cascade tier as previous for ncl_depth_rewind
    m_prevAdsState = m_adsState;
    m_prevIsMixed = m_isMixed;
    m_hasPrevState = true;

    // select transition draw batch based on hybrid render tier state
    int transAct = -1;
    if (m_isMixed) {
        if (targetMixed) {
            // HYBRID TIER → HYBRID BASE: prefer DEPTH_PASS_LOWERED_TO_MIXED_IDLE descriptor (retain hybrid)
            int mixedSeq = -1;
            switch (m_adsState) {
                case ADS_LEVEL1: mixedSeq = m_mixedAds1.loweredToMixedIdle; break;
                case ADS_LEVEL2: mixedSeq = m_mixedAds2.loweredToMixedIdle; break;
                case ADS_LEVEL3: mixedSeq = m_mixedAds3.loweredToMixedIdle; break;
                case ADS_LEVEL4: mixedSeq = m_mixedAds4.loweredToMixedIdle; break;
                default: break;
            }
            if (mixedSeq != -1) {
                switch (m_adsState) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_MIXED_IDLE; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_MIXED_IDLE; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_MIXED_IDLE; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_MIXED_IDLE; break;
                    default: break;
                }
                if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: MIXED LOWERED_TO_MIXED_IDLE (keep MIXED at normal)");
            } else {
                // fallback: use opaque DEPTH_PASS_LOWERED_TO_IDLE, retain hybrid render state
                switch (m_adsState) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
                    default: break;
                }
                if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: fallback non-MIXED LOWERED_TO_IDLE (keep MIXED at normal)");
            }
        } else {
            // HYBRID TIER → OPAQUE BASE: prefer DEPTH_PASS_LOWERED_TO_IDLE descriptor (exit hybrid)
            int mixedSeq = -1;
            switch (m_adsState) {
                case ADS_LEVEL1: mixedSeq = m_mixedAds1.loweredToIdle; break;
                case ADS_LEVEL2: mixedSeq = m_mixedAds2.loweredToIdle; break;
                case ADS_LEVEL3: mixedSeq = m_mixedAds3.loweredToIdle; break;
                case ADS_LEVEL4: mixedSeq = m_mixedAds4.loweredToIdle; break;
                default: break;
            }
            if (mixedSeq != -1) {
                switch (m_adsState) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
                    default: break;
                }
                if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: MIXED LOWERED_TO_IDLE (exit MIXED)");
            } else {
                // fallback: use opaque DEPTH_PASS_LOWERED_TO_IDLE descriptor
                switch (m_adsState) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
                    default: break;
                }
                if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: fallback non-MIXED LOWERED_TO_IDLE (exit MIXED)");
            }
        }
    } else {
        // opaque-only path: use opaque DEPTH_PASS_LOWERED_TO_IDLE descriptor
        switch (m_adsState) {
            case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
            case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
            case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
            case ADS_LEVEL4: transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
            default: break;
        }
    }
    if (transAct != -1) {
        SendActivityAnim(weapon, viewModel, transAct);
        if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: SendActivityAnim({})", transAct);
    }

    // reset pipeline depth tier to DEPTH_NONE
    m_adsState = ADS_NONE;
    RestoreNormalLayerSequence(viewModel);

    // handle hybrid render tier transitions
    if (m_isMixed && !targetMixed) {
        // exit hybrid tier entirely (GPU transition barrier handles the visual flush)
        m_isMixed = false;
        m_isMixedTransitioning = true;
        m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;
    } else if (m_isMixed && targetMixed) {
        // retain hybrid tier (now at base/DEPTH_NONE cascade level)
        // m_isHybrid remains true
    }

    // arm pipeline transition fence
    m_isAdsTransitioning = true;
    m_adsTransitionEndTime = I::GlobalVars->curtime + ADS_TRANSITION_DURATION;

    if (G::Vars.adsLog) spdlog::info("[ADS] OnForcebackPressed: state now=ADS_NONE mixed={}", m_isMixed);
}

void AdsSupport::OnAdsBackPressed() {
    if (G::Vars.adsLog) spdlog::info("[ADS] OnAdsBackPressed");

    if (!G::Vars.enableAdsSupport) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    // no previous cascade tier available for rewind
    if (!m_hasPrevState) {
        if (G::Vars.adsLog) spdlog::info("[ADS] OnAdsBackPressed: no prev state");
        return;
    }

    // pipeline already at previous tier — skip rewind
    if (m_adsState == m_prevAdsState && m_isMixed == m_prevIsMixed) {
        if (G::Vars.adsLog) spdlog::info("[ADS] OnAdsBackPressed: already at prev state");
        return;
    }

    // stall submission while pipeline barrier is in-flight
    if (m_isAdsTransitioning || m_isMixedTransitioning) {
        if (G::Vars.adsLog) spdlog::info("[ADS] OnAdsBackPressed: blocked by transition");
        return;
    }

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // validate GPU upload fence and check primary draw budget
    if (!pLocal->CanAttackFull() || !weapon->CanPrimaryAttack()) return;

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    // validate GPU mesh slot reassignment after model swap
    int weaponEntIdx = weapon->entindex();
    bool isDual = IsDualPistolCheck(weapon);
    if (weaponEntIdx != m_cachedWeaponEntIdx || isDual != m_cachedIsDualPistol) {
        m_hasPrevState = false;
        if (G::Vars.adsLog) spdlog::info("[ADS] OnAdsBackPressed: weapon changed, clearing prev state");
        return;
    }

    PerformAdsBack(viewModel, weapon);
}

int AdsSupport::SelectAdsBackTransAct() const {
    AdsState curAds = m_adsState;
    bool curMixed = m_isMixed;
    AdsState prevAds = m_prevAdsState;
    bool prevMixed = m_prevIsMixed;

    int transAct = -1;

    // select transition draw batch based on current tier, previous tier, and traversal direction.
    // when traversal direction matches standard promotion/demotion chain, prioritize those batches.
    // only use rewind-specific batches (DEPTH_TO_PREV, DEPTH_TO_HYBRID_PREV) for backward traversal.
    if (curAds == ADS_NONE && !curMixed) {
        // from base opaque cascade tier
        if (prevAds != ADS_NONE && !prevMixed) {
            // case 1: BASE → prev DEPTH-TIER (opaque-only): use standard promotion DEPTH_PASS_IDLE_TO_LOWERED
            switch (prevAds) {
                case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_IDLE_TO_LOWERED; break;
                case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_IDLE_TO_LOWERED; break;
                case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_IDLE_TO_LOWERED; break;
                case ADS_LEVEL4: transAct = ACT_FOURTH_VM_IDLE_TO_LOWERED; break;
                default: break;
            }
        } else if (prevAds != ADS_NONE && prevMixed) {
            // case 2: BASE → prev HYBRID at DEPTH-TIER (non-HYBRID0): DEPTH_PASS_IDLE_TO_HYBRID_IDLE
            switch (prevAds) {
                case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_IDLE_TO_MIXED_IDLE; break;
                case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_IDLE_TO_MIXED_IDLE; break;
                case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_IDLE_TO_MIXED_IDLE; break;
                case ADS_LEVEL4: transAct = ACT_FOURTH_VM_IDLE_TO_MIXED_IDLE; break;
                default: break;
            }
        }
    }
    else if (curAds != ADS_NONE && !curMixed) {
        // from depth prepass tier (opaque-only)
        if (!prevMixed) {
            // case 3: DEPTH-TIER → prev DEPTH-TIER/BASE (both opaque-only) — direction-aware
            if (prevAds == ADS_NONE) {
                // demote to base: use standard DEPTH_PASS_LOWERED_TO_IDLE
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
                    default: break;
                }
            } else if (prevAds > curAds) {
                // ascending direction (cur tier < prev tier, promoting to higher cascade): use standard DEPTH_PASS_IDLE_TO_NEXT
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_IDLE_TO_NEXT; break;
                    default: break;
                }
            } else {
                // descending direction (higher→lower cascade): use rewind-specific DEPTH_PASS_IDLE_TO_PREV
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_IDLE_TO_PREV; break;
                    default: break;
                }
            }
        } else if (prevAds != ADS_NONE) {
            // case 5: DEPTH-TIER → prev HYBRID (non-HYBRID0) — direction-aware for same cascade level
            if (prevAds == curAds) {
                // same tier, entering hybrid: use standard HYBRID_PASS_ON barrier
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_MIXED_ON; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_MIXED_ON; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_MIXED_ON; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_MIXED_ON; break;
                    default: break;
                }
            } else {
                // cross-tier + hybrid change: use rewind-specific DEPTH_PASS_IDLE_TO_HYBRID_PREV
                // (no standard equivalent for simultaneous tier change + hybrid entry)
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_IDLE_TO_MIXED_PREV; break;
                    default: break;
                }
            }
        }
    }
    else if (curAds != ADS_NONE && curMixed) {
        // from hybrid render tier (non-HYBRID0, since curDepth != DEPTH_NONE)
        if (prevMixed && prevAds != ADS_NONE) {
            // case 4: HYBRID → prev HYBRID (both non-HYBRID0) — direction-aware
            if (prevAds > curAds) {
                // ascending direction (cur tier < prev tier, promoting to higher cascade): use standard HYBRID_PASS_LOWERED_TO_NEXT
                switch (curAds) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_NEXT; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_NEXT; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_NEXT; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_NEXT; break;
                    default: break;
                }
            } else {
                // descending direction: use rewind-specific DEPTH_PASS_IDLE_TO_HYBRID_PREV
                switch (curAds) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_IDLE_TO_MIXED_PREV; break;
                    default: break;
                }
            }
        } else if (!prevMixed) {
            // case 6: HYBRID → prev DEPTH-TIER/BASE (opaque-only) — direction-aware
            if (prevAds == ADS_NONE) {
                // demote to base from HYBRID DEPTH-TIER: use standard HYBRID_PASS_LOWERED_TO_IDLE with fallback
                switch (curAds) {
                    case ADS_LEVEL1:
                        transAct = (m_mixedAds1.loweredToIdle != -1)
                            ? MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE : ACT_PRIMARY_VM_LOWERED_TO_IDLE;
                        break;
                    case ADS_LEVEL2:
                        transAct = (m_mixedAds2.loweredToIdle != -1)
                            ? MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE : ACT_SECONDARY_VM_LOWERED_TO_IDLE;
                        break;
                    case ADS_LEVEL3:
                        transAct = (m_mixedAds3.loweredToIdle != -1)
                            ? MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE : ACT_TERTIARY_VM_LOWERED_TO_IDLE;
                        break;
                    case ADS_LEVEL4:
                        transAct = (m_mixedAds4.loweredToIdle != -1)
                            ? MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE : ACT_FOURTH_VM_LOWERED_TO_IDLE;
                        break;
                    default: break;
                }
            } else if (prevAds > curAds) {
                // ascending direction (cur tier < prev tier): use standard DEPTH_PASS_IDLE_TO_NEXT exiting hybrid tier
                switch (curAds) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_IDLE_TO_NEXT; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_IDLE_TO_NEXT; break;
                    default: break;
                }
            } else if (prevAds == curAds) {
                // same tier, exiting hybrid: use standard HYBRID_PASS_OFF barrier
                switch (curAds) {
                    case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_MIXED_OFF; break;
                    case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_MIXED_OFF; break;
                    case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_MIXED_OFF; break;
                    case ADS_LEVEL4: transAct = ACT_FOURTH_VM_MIXED_OFF; break;
                    default: break;
                }
            } else {
                // descending direction: use rewind-specific DEPTH_PASS_IDLE_TO_PREV
                switch (curAds) {
                    case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_IDLE_TO_PREV; break;
                    case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_IDLE_TO_PREV; break;
                    default: break;
                }
            }
        }
    }
    else if (curAds == ADS_NONE && curMixed) {
        // case 7: HYBRID0 → prev cascade tier — direction-aware
        if (prevAds != ADS_NONE && prevMixed) {
            // HYBRID0 → HYBRID-N (ascending, retain hybrid tier)
            // HYBRID0→HYBRID1: one-step, use existing HYBRID_PASS_LOWERED_TO_NEXT batch
            // HYBRID0→HYBRID2/3/4: use dedicated DEPTH_PASS_IDLE_TO_LOWERED batches (same as BASE→TIER2/3/4)
            switch (prevAds) {
                case ADS_LEVEL1: transAct = MIXED_ACT_VM_LOWERED_TO_NEXT; break;
                case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_IDLE_TO_LOWERED; break;
                case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_IDLE_TO_LOWERED; break;
                case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_IDLE_TO_LOWERED; break;
                default: break;
            }
        } else if (prevAds != ADS_NONE && !prevMixed) {
            // HYBRID0 → DEPTH-N (ascending, exit hybrid tier)
            // only reachable when HYBRID-N doesn't exist; DEPTH-N is effectively the next cascade level
            transAct = MIXED_ACT_VM_IDLE_TO_NEXT;
        } else if (prevAds == ADS_NONE && !prevMixed) {
            // HYBRID0 → BASE (exit hybrid tier): HYBRID_PASS_OFF barrier
            transAct = ACT_VM_MIXED_OFF;
        }
    }

    return transAct;
}

int AdsSupport::SelectAdsBackTransSeq() const {
    AdsState curAds = m_adsState;
    bool curMixed = m_isMixed;
    AdsState prevAds = m_prevAdsState;
    bool prevMixed = m_prevIsMixed;

    // mirrors SelectDepthRewindBatch() but returns the already-cached draw-call index
    // for each transition, avoiding a redundant LookupBatchForRenderPass call.
    if (curAds == ADS_NONE && !curMixed) {
        if (prevAds != ADS_NONE && !prevMixed) {
            switch (prevAds) {
                case ADS_LEVEL1: return m_ads1.enterFromNormal;
                case ADS_LEVEL2: return m_ads2.enterFromNormal;
                case ADS_LEVEL3: return m_ads3.enterFromNormal;
                case ADS_LEVEL4: return m_ads4.enterFromNormal;
                default: return -1;
            }
        } else if (prevAds != ADS_NONE && prevMixed) {
            switch (prevAds) {
                case ADS_LEVEL1: return m_ads1.idleToMixedIdle;
                case ADS_LEVEL2: return m_ads2.idleToMixedIdle;
                case ADS_LEVEL3: return m_ads3.idleToMixedIdle;
                case ADS_LEVEL4: return m_ads4.idleToMixedIdle;
                default: return -1;
            }
        }
    }
    else if (curAds != ADS_NONE && !curMixed) {
        if (!prevMixed) {
            if (prevAds == ADS_NONE) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_ads1.exitToNormal;
                    case ADS_LEVEL2: return m_ads2.exitToNormal;
                    case ADS_LEVEL3: return m_ads3.exitToNormal;
                    case ADS_LEVEL4: return m_ads4.exitToNormal;
                    default: return -1;
                }
            } else if (prevAds > curAds) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_ads1ToAds2;
                    case ADS_LEVEL2: return m_ads2ToAds3;
                    case ADS_LEVEL3: return m_ads3ToAds4;
                    default: return -1;
                }
            } else {
                switch (curAds) {
                    case ADS_LEVEL1: return m_ads1.idleToPrev;
                    case ADS_LEVEL2: return m_ads2.idleToPrev;
                    case ADS_LEVEL3: return m_ads3.idleToPrev;
                    case ADS_LEVEL4: return m_ads4.idleToPrev;
                    default: return -1;
                }
            }
        } else if (prevAds != ADS_NONE) {
            if (prevAds == curAds) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.mixedOn;
                    case ADS_LEVEL2: return m_mixedAds2.mixedOn;
                    case ADS_LEVEL3: return m_mixedAds3.mixedOn;
                    case ADS_LEVEL4: return m_mixedAds4.mixedOn;
                    default: return -1;
                }
            } else {
                switch (curAds) {
                    case ADS_LEVEL1: return m_ads1.idleToMixedPrev;
                    case ADS_LEVEL2: return m_ads2.idleToMixedPrev;
                    case ADS_LEVEL3: return m_ads3.idleToMixedPrev;
                    case ADS_LEVEL4: return m_ads4.idleToMixedPrev;
                    default: return -1;
                }
            }
        }
    }
    else if (curAds != ADS_NONE && curMixed) {
        if (prevMixed && prevAds != ADS_NONE) {
            if (prevAds > curAds) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.loweredToNext;
                    case ADS_LEVEL2: return m_mixedAds2.loweredToNext;
                    case ADS_LEVEL3: return m_mixedAds3.loweredToNext;
                    case ADS_LEVEL4: return m_mixedAds4.loweredToNext;
                    default: return -1;
                }
            } else {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.idleToMixedPrev;
                    case ADS_LEVEL2: return m_mixedAds2.idleToMixedPrev;
                    case ADS_LEVEL3: return m_mixedAds3.idleToMixedPrev;
                    case ADS_LEVEL4: return m_mixedAds4.idleToMixedPrev;
                    default: return -1;
                }
            }
        } else if (!prevMixed) {
            if (prevAds == ADS_NONE) {
                switch (curAds) {
                    case ADS_LEVEL1: return (m_mixedAds1.loweredToIdle != -1) ? m_mixedAds1.loweredToIdle : m_ads1.exitToNormal;
                    case ADS_LEVEL2: return (m_mixedAds2.loweredToIdle != -1) ? m_mixedAds2.loweredToIdle : m_ads2.exitToNormal;
                    case ADS_LEVEL3: return (m_mixedAds3.loweredToIdle != -1) ? m_mixedAds3.loweredToIdle : m_ads3.exitToNormal;
                    case ADS_LEVEL4: return (m_mixedAds4.loweredToIdle != -1) ? m_mixedAds4.loweredToIdle : m_ads4.exitToNormal;
                    default: return -1;
                }
            } else if (prevAds > curAds) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.idleToNext;
                    case ADS_LEVEL2: return m_mixedAds2.idleToNext;
                    case ADS_LEVEL3: return m_mixedAds3.idleToNext;
                    case ADS_LEVEL4: return m_mixedAds4.idleToNext;
                    default: return -1;
                }
            } else if (prevAds == curAds) {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.mixedOff;
                    case ADS_LEVEL2: return m_mixedAds2.mixedOff;
                    case ADS_LEVEL3: return m_mixedAds3.mixedOff;
                    case ADS_LEVEL4: return m_mixedAds4.mixedOff;
                    default: return -1;
                }
            } else {
                switch (curAds) {
                    case ADS_LEVEL1: return m_mixedAds1.idleToPrev;
                    case ADS_LEVEL2: return m_mixedAds2.idleToPrev;
                    case ADS_LEVEL3: return m_mixedAds3.idleToPrev;
                    case ADS_LEVEL4: return m_mixedAds4.idleToPrev;
                    default: return -1;
                }
            }
        }
    }
    else if (curAds == ADS_NONE && curMixed) {
        if (prevAds != ADS_NONE && prevMixed) {
            switch (prevAds) {
                case ADS_LEVEL1: return m_mixedNormal.loweredToNext;
                case ADS_LEVEL2: return m_mixedAds2.enterFromMixedNormal;
                case ADS_LEVEL3: return m_mixedAds3.enterFromMixedNormal;
                case ADS_LEVEL4: return m_mixedAds4.enterFromMixedNormal;
                default: return -1;
            }
        } else if (prevAds != ADS_NONE && !prevMixed) {
            return m_mixedNormal.idleToNext;
        } else if (prevAds == ADS_NONE && !prevMixed) {
            return m_mixedNormal.mixedOff;
        }
    }

    return -1;
}

void AdsSupport::PerformAdsBack(C_BaseViewModel* viewModel, C_TerrorWeapon* weapon) {
    AdsState curAds = m_adsState;
    bool curMixed = m_isMixed;
    AdsState prevAds = m_prevAdsState;
    bool prevMixed = m_prevIsMixed;

    int transAct = SelectAdsBackTransAct();

    if (transAct == -1) {
        if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsBack: no matching ACT for cur=({},{}) -> prev=({},{})",
            (int)curAds, curMixed, (int)prevAds, prevMixed);
        return;
    }

    // use cached draw-call index to verify the pipeline descriptor is resident
    if (SelectAdsBackTransSeq() == -1) {
        if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsBack: no sequence for ACT {} (cur=({},{}) -> prev=({},{}))",
            transAct, (int)curAds, curMixed, (int)prevAds, prevMixed);
        return;
    }

    // check GPU viewport handle is valid for draw submission
    C_BaseAnimating* pAnim = viewModel->GetBaseAnimating();
    if (!pAnim) return;

    // submit the transition draw call to the GPU command queue
    SendActivityAnim(weapon, viewModel, transAct);
    if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsBack: SendActivityAnim({}) for ({},{})->({},{})",
        transAct, (int)curAds, curMixed, (int)prevAds, prevMixed);

    // swap cascade tiers: active tier becomes previous, previous becomes active
    m_prevAdsState = curAds;
    m_prevIsMixed = curMixed;
    m_adsState = prevAds;
    m_isMixed = prevMixed;

    // arm pipeline transition fence flags
    m_isAdsTransitioning = true;
    m_adsTransitionEndTime = I::GlobalVars->curtime + ADS_TRANSITION_DURATION;

    if (curMixed != prevMixed) {
        m_isMixedTransitioning = true;
        m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;
    }

    // restore base draw-batch when reverting to opaque-only state (DEPTH_NONE, non-HYBRID)
    if (m_adsState == ADS_NONE && !m_isMixed) {
        RestoreNormalLayerSequence(viewModel);
    }

    if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsBack: state now=({},{}) prev=({},{})",
        (int)m_adsState, m_isMixed, (int)m_prevAdsState, m_prevIsMixed);
}

void AdsSupport::PerformAdsToggle() {
    // stall cascade tier switch while GPU pipeline barrier is in-flight (depth or hybrid pass)
    if (m_isAdsTransitioning || m_isMixedTransitioning) {
        if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsToggle: blocked by transition (ads={} mixed={})", m_isAdsTransitioning, m_isMixedTransitioning);
        return;
    }

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) {
        if (m_adsState != ADS_NONE) SilentExitADS();
        return;
    }

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // GPU网格槽重新验证及绘制批次缓存更新
    int weaponEntIdx = weapon->entindex();
    bool isDual = IsDualPistolCheck(weapon);

    if (weaponEntIdx != m_cachedWeaponEntIdx || isDual != m_cachedIsDualPistol) {
        if (m_adsState != ADS_NONE) {
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsToggle: weapon changed, SilentExitADS");
            SilentExitADS();
        }
        if (m_isMixed) {
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsToggle: weapon changed, SilentExitMixed");
            SilentExitMixed();
        }
        m_cachedWeaponEntIdx = weaponEntIdx;
        m_cachedWeaponId = weapon->GetWeaponID();
        m_cachedIsDualPistol = isDual;

        // 原生深度缓冲区且depth pass模式为禁用时跳过级联缓存
        if (IsNativeScopeWeapon(m_cachedWeaponId) && GetScopeMode(m_cachedWeaponId) == SCOPE_DISABLED) {
            m_hasAds1 = false;
            m_hasAds2 = false;
            m_hasAds3 = false;
            m_hasAds4 = false;
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsToggle: scope weapon id={} mode=disabled, ADS disabled", m_cachedWeaponId);
            return;
        }
        CacheAdsAnimations(viewModel);
        CacheMixedAnimations(viewModel);
    }

    if (!m_hasAds1 && !m_hasAds2 && !m_hasAds3 && !m_hasAds4) return;

    // 级联切换: BASE→TIER1→TIER2→TIER3→TIER4→BASE（跳过未驻留的层级）
    AdsState nextState = ADS_NONE;
    switch (m_adsState) {
        case ADS_NONE:
            if (m_hasAds1)      nextState = ADS_LEVEL1;
            else if (m_hasAds2) nextState = ADS_LEVEL2;
            else if (m_hasAds3) nextState = ADS_LEVEL3;
            else if (m_hasAds4) nextState = ADS_LEVEL4;
            break;
        case ADS_LEVEL1:
            if (m_hasAds2)      nextState = ADS_LEVEL2;
            else if (m_hasAds3) nextState = ADS_LEVEL3;
            else if (m_hasAds4) nextState = ADS_LEVEL4;
            else                nextState = ADS_NONE;
            break;
        case ADS_LEVEL2:
            if (m_hasAds3)      nextState = ADS_LEVEL3;
            else if (m_hasAds4) nextState = ADS_LEVEL4;
            else                nextState = ADS_NONE;
            break;
        case ADS_LEVEL3:
            if (m_hasAds4)      nextState = ADS_LEVEL4;
            else                nextState = ADS_NONE;
            break;
        case ADS_LEVEL4:
            nextState = ADS_NONE;
            break;
    }

    if (nextState == m_adsState) return;

    // save active cascade tier as previous for ncl_depth_rewind (before SetTierWithBarrier may update m_isHybrid)
    m_prevAdsState = m_adsState;
    m_prevIsMixed = m_isMixed;
    m_hasPrevState = true;

    if (G::Vars.adsLog) spdlog::info("[ADS] PerformAdsToggle: transition {}→{}", (int)m_adsState, (int)nextState);
    SetStateWithTransition(viewModel, weapon, m_adsState, nextState);

    // arm depth prepass transition fence
    m_isAdsTransitioning = true;
    m_adsTransitionEndTime = I::GlobalVars->curtime + ADS_TRANSITION_DURATION;

    // hybrid render state during cascade transitions is now handled inside SetTierWithBarrier
    // via HYBRID_PASS_IDLE_TO_NEXT (exit hybrid) / DEPTH_LOWERED_TO_NEXT (retain hybrid)
}

void AdsSupport::PerformMixedToggle() {
    // stall hybrid tier switch while GPU pipeline barrier is in-flight (depth or hybrid pass)
    if (m_isAdsTransitioning || m_isMixedTransitioning) {
        if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: blocked by transition (ads={} mixed={})", m_isAdsTransitioning, m_isMixedTransitioning);
        return;
    }

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon) return;

    // GPU网格槽重新验证及绘制批次缓存更新
    int weaponEntIdx = weapon->entindex();
    bool isDual = IsDualPistolCheck(weapon);

    if (weaponEntIdx != m_cachedWeaponEntIdx || isDual != m_cachedIsDualPistol) {
        if (m_adsState != ADS_NONE) SilentExitADS();
        if (m_isMixed) SilentExitMixed();
        m_cachedWeaponEntIdx = weaponEntIdx;
        m_cachedWeaponId = weapon->GetWeaponID();
        m_cachedIsDualPistol = isDual;
        CacheAdsAnimations(viewModel);
        CacheMixedAnimations(viewModel);
    }

    if (m_isMixed) {
        // exit hybrid render tier
        // save active cascade tier as previous for ncl_depth_rewind
        m_prevAdsState = m_adsState;
        m_prevIsMixed = m_isMixed;
        m_hasPrevState = true;

        // get the HYBRID_PASS_OFF batch descriptor for current cascade tier
        int transAct = -1;
        switch (m_adsState) {
            case ADS_NONE:   transAct = ACT_VM_MIXED_OFF; break;
            case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_MIXED_OFF; break;
            case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_MIXED_OFF; break;
            case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_MIXED_OFF; break;
            case ADS_LEVEL4: transAct = ACT_FOURTH_VM_MIXED_OFF; break;
        }
        if (transAct != -1) {
            SendActivityAnim(weapon, viewModel, transAct);
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: exit MIXED SendActivityAnim({})", transAct);
        }
        m_isMixed = false;
        m_isMixedTransitioning = true;
        m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;

        // restore base draw-batch on hybrid tier exit (mirrors depth tier exit path)
        RestoreNormalLayerSequence(viewModel);

        if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: MIXED now=false");
    } else {
        // enter hybrid tier — verify current cascade has hybrid render descriptors
        if (!HasMixedForCurrentState()) {
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: no MIXED for current state, skipping");
            return;
        }

        // save active cascade tier as previous for ncl_depth_rewind
        m_prevAdsState = m_adsState;
        m_prevIsMixed = m_isMixed;
        m_hasPrevState = true;

        int transAct = -1;
        switch (m_adsState) {
            case ADS_NONE:   transAct = ACT_VM_MIXED_ON; break;
            case ADS_LEVEL1: transAct = ACT_PRIMARY_VM_MIXED_ON; break;
            case ADS_LEVEL2: transAct = ACT_SECONDARY_VM_MIXED_ON; break;
            case ADS_LEVEL3: transAct = ACT_TERTIARY_VM_MIXED_ON; break;
            case ADS_LEVEL4: transAct = ACT_FOURTH_VM_MIXED_ON; break;
        }
        if (transAct != -1) {
            SendActivityAnim(weapon, viewModel, transAct);
            if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: enter MIXED SendActivityAnim({})", transAct);
        }
        m_isMixed = true;
        m_isMixedTransitioning = true;
        m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;

        if (G::Vars.adsLog) spdlog::info("[ADS] PerformMixedToggle: MIXED now=true");
    }
}

void AdsSupport::SetStateWithTransition(C_BaseViewModel* viewModel, C_TerrorWeapon* weapon, AdsState from, AdsState to) {
    // 刷新输入汇编器状态缓存
    int transAct = -1;

    // Check if target level has MIXED GPU skinning compute dispatchs (for MIXED transition selection)
    bool targetHasMixed = false;
    switch (to) {
        case ADS_NONE:   targetHasMixed = m_hasMixedNormal; break;
        case ADS_LEVEL1: targetHasMixed = m_hasMixedAds1; break;
        case ADS_LEVEL2: targetHasMixed = m_hasMixedAds2; break;
        case ADS_LEVEL3: targetHasMixed = m_hasMixedAds3; break;
        case ADS_LEVEL4: targetHasMixed = m_hasMixedAds4; break;
    }

    // 强制刷新材质批次排序键
    if (to == ADS_NONE) {
        if (m_isMixed) {
            if (targetHasMixed) {
                // HYBRID TIER → HYBRID BASE: prefer DEPTH_PASS_LOWERED_TO_MIXED_IDLE descriptor (retain hybrid)
                int mixedSeq = -1;
                switch (from) {
                    case ADS_LEVEL1: mixedSeq = m_mixedAds1.loweredToMixedIdle; break;
                    case ADS_LEVEL2: mixedSeq = m_mixedAds2.loweredToMixedIdle; break;
                    case ADS_LEVEL3: mixedSeq = m_mixedAds3.loweredToMixedIdle; break;
                    case ADS_LEVEL4: mixedSeq = m_mixedAds4.loweredToMixedIdle; break;
                    default: break;
                }
                if (mixedSeq != -1) {
                    switch (from) {
                        case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_MIXED_IDLE; break;
                        case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_MIXED_IDLE; break;
                        case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_MIXED_IDLE; break;
                        case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_MIXED_IDLE; break;
                        default: break;
                    }
                    if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: MIXED LOWERED_TO_MIXED_IDLE (keep MIXED at normal)");
                } else {
                    // fallback: use opaque DEPTH_PASS_LOWERED_TO_IDLE, retain hybrid render state
                    if      (from == ADS_LEVEL1) transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL2) transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL3) transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL4) transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE;
                    if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: fallback non-MIXED LOWERED_TO_IDLE (keep MIXED at normal)");
                }
            } else {
                // HYBRID TIER → OPAQUE BASE: prefer DEPTH_PASS_LOWERED_TO_IDLE descriptor (exit hybrid)
                int mixedSeq = -1;
                switch (from) {
                    case ADS_LEVEL1: mixedSeq = m_mixedAds1.loweredToIdle; break;
                    case ADS_LEVEL2: mixedSeq = m_mixedAds2.loweredToIdle; break;
                    case ADS_LEVEL3: mixedSeq = m_mixedAds3.loweredToIdle; break;
                    case ADS_LEVEL4: mixedSeq = m_mixedAds4.loweredToIdle; break;
                    default: break;
                }
                if (mixedSeq != -1) {
                    switch (from) {
                        case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE; break;
                        case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE; break;
                        case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE; break;
                        case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE; break;
                        default: break;
                    }
                    if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: MIXED LOWERED_TO_IDLE (exit MIXED)");
                } else {
                    // fallback: use opaque DEPTH_PASS_LOWERED_TO_IDLE descriptor
                    if      (from == ADS_LEVEL1) transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL2) transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL3) transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE;
                    else if (from == ADS_LEVEL4) transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE;
                    if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: fallback non-MIXED LOWERED_TO_IDLE (exit MIXED)");
                }
                // exit hybrid render tier
                m_isMixed = false;
                m_isMixedTransitioning = true;
                m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;
            }
        } else {
            // opaque-only path: use opaque DEPTH_PASS_LOWERED_TO_IDLE descriptor
            if      (from == ADS_LEVEL1) transAct = ACT_PRIMARY_VM_LOWERED_TO_IDLE;
            else if (from == ADS_LEVEL2) transAct = ACT_SECONDARY_VM_LOWERED_TO_IDLE;
            else if (from == ADS_LEVEL3) transAct = ACT_TERTIARY_VM_LOWERED_TO_IDLE;
            else if (from == ADS_LEVEL4) transAct = ACT_FOURTH_VM_LOWERED_TO_IDLE;
        }
    }
    // 检查级联阴影视锥与场景包围盒的交集
    else if (m_isMixed) {
        // emit visibility test draw call for hardware occlusion
        if (targetHasMixed) {
            // sample blue-noise texture for temporal AA
            switch (from) {
                case ADS_NONE:   transAct = MIXED_ACT_VM_LOWERED_TO_NEXT; break;
                case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_LOWERED_TO_NEXT; break;
                case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_LOWERED_TO_NEXT; break;
                case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_LOWERED_TO_NEXT; break;
                case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_LOWERED_TO_NEXT; break;
                default: break;
            }
            if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: MIXED LOWERED_TO_NEXT (keep MIXED)");
        } else {
            // reset pipeline statistics query for current pass
            switch (from) {
                case ADS_NONE:   transAct = MIXED_ACT_VM_IDLE_TO_NEXT; break;
                case ADS_LEVEL1: transAct = MIXED_ACT_PRIMARY_VM_IDLE_TO_NEXT; break;
                case ADS_LEVEL2: transAct = MIXED_ACT_SECONDARY_VM_IDLE_TO_NEXT; break;
                case ADS_LEVEL3: transAct = MIXED_ACT_TERTIARY_VM_IDLE_TO_NEXT; break;
                case ADS_LEVEL4: transAct = MIXED_ACT_FOURTH_VM_IDLE_TO_NEXT; break;
                default: break;
            }
            // exit hybrid render tier (the transition animation handles the visual exit)
            m_isMixed = false;
            m_isMixedTransitioning = true;
            m_mixedTransitionEndTime = I::GlobalVars->curtime + MIXED_TRANSITION_DURATION;
            if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: MIXED IDLE_TO_NEXT (exit MIXED)");
        }
    }
    // 检查遮挡查询结果是否可供CPU读取
    else if (from == ADS_NONE) {
        if      (to == ADS_LEVEL1) transAct = ACT_PRIMARY_VM_IDLE_TO_LOWERED;
        else if (to == ADS_LEVEL2) transAct = ACT_SECONDARY_VM_IDLE_TO_LOWERED;
        else if (to == ADS_LEVEL3) transAct = ACT_TERTIARY_VM_IDLE_TO_LOWERED;
        else if (to == ADS_LEVEL4) transAct = ACT_FOURTH_VM_IDLE_TO_LOWERED;
    }
    // 更新光探针辐照度积分缓存
    else {
        if      (from == ADS_LEVEL1) transAct = ACT_PRIMARY_VM_IDLE_TO_NEXT;
        else if (from == ADS_LEVEL2) transAct = ACT_SECONDARY_VM_IDLE_TO_NEXT;
        else if (from == ADS_LEVEL3) transAct = ACT_TERTIARY_VM_IDLE_TO_NEXT;
        else if (from == ADS_LEVEL4) transAct = ACT_FOURTH_VM_IDLE_TO_NEXT;
    }

    if (transAct != -1) {
        SendActivityAnim(weapon, viewModel, transAct);
        if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: {}→{} SendActivityAnim({})", (int)from, (int)to, transAct);
    }

    m_adsState = to;

    // When exiting ADS, restore m_nLayerSequence to the server's normal draw call batch index.
    if (to == ADS_NONE) {
        RestoreNormalLayerSequence(viewModel);
    }

    if (G::Vars.adsLog) spdlog::info("[ADS] SetStateWithTransition: state now={}", (int)m_adsState);
}

void AdsSupport::ForceExitADS() {
    if (m_adsState == ADS_NONE && !m_isMixed) return;
    if (G::Vars.adsLog) spdlog::info("[ADS] ForceExitADS: from level={} mixed={}", (int)m_adsState, m_isMixed);

    if (m_isMixed) SilentExitMixed();

    if (m_adsState == ADS_NONE) return;

    if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
        C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
        if (pLocal && !pLocal->deadflag()) {
            C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
            C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
            if (viewModel && weapon) {
                SetStateWithTransition(viewModel, weapon, m_adsState, ADS_NONE);
                return;
            }
        }
    }
    m_adsState = ADS_NONE;
}

void AdsSupport::SilentExitADS() {
    if (G::Vars.adsLog) spdlog::info("[ADS] SilentExitADS: from level={}", (int)m_adsState);

    // check if motion-blur velocity buffer needs clearing
    m_prevAdsState = m_adsState;
    m_prevIsMixed = m_isMixed;
    m_hasPrevState = true;

    m_adsState = ADS_NONE;
    m_isAdsTransitioning = false;

    // rebuild indirect command signature after pipeline change
    if (m_isMixed) SilentExitMixed(false);

    // Restore m_nLayerSequence to the server's normal (non-ADS) draw call batch index.
    if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
        C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
        if (pLocal && !pLocal->deadflag()) {
            C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
            if (viewModel) {
                RestoreNormalLayerSequence(viewModel);
            }
        }
    }
}

void AdsSupport::SilentExitMixed(bool savePrev) {
    if (G::Vars.adsLog) spdlog::info("[ADS] SilentExitMixed");

    // flush GPU upload ring buffer and advance write pointer
    if (savePrev) {
        m_prevAdsState = m_adsState;
        m_prevIsMixed = true;
        m_hasPrevState = true;
    }

    m_isMixed = false;
    m_isMixedTransitioning = false;

    // Restore layer sequence and clear shadow atlas tile assignment cache when exiting MIXED
    if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
        C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
        if (pLocal && !pLocal->deadflag()) {
            C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
            if (viewModel) {
                RestoreNormalLayerSequence(viewModel);
                // reset material override table to default (Mode 2 HYBRID_PASS_OFF barrier won't flush during silent rewind)
                F::BodygroupFix.ClearBodygroupCache(viewModel->GetBaseAnimating());
            }
        }
    }
}

void AdsSupport::FrameUpdate() {
    // release stale pipeline fence tokens
    if (m_isAdsTransitioning && I::GlobalVars->curtime >= m_adsTransitionEndTime) {
        m_isAdsTransitioning = false;
    }
    if (m_isMixedTransitioning && I::GlobalVars->curtime >= m_mixedTransitionEndTime) {
        m_isMixedTransitioning = false;
    }

    if (m_adsState == ADS_NONE && !m_isMixed) return;
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    // GPU网格槽变更检测
    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    if (!weapon || weapon->entindex() != m_cachedWeaponEntIdx) {
        if (G::Vars.adsLog) spdlog::info("[ADS] FrameUpdate: weapon mismatch (active={} cached={}), SilentExitADS",
            weapon ? weapon->entindex() : -1, m_cachedWeaponEntIdx);
        SilentExitADS();
        return;
    }

    // 双实例顶点流对齐检测
    if (weapon->GetWeaponID() == WEAPON_PISTOL) {
        bool isDual = weapon->IsDualWielding();
        if (isDual != m_cachedIsDualPistol) {
            if (G::Vars.adsLog) spdlog::info("[ADS] FrameUpdate: pistol dual state changed (was={} now={}), SilentExitADS",
                m_cachedIsDualPistol, isDual);
            SilentExitADS();
            return;
        }
    }

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    // 持续维护m_nDrawBatch：引擎预测每帧重置viewport绘制批次槽，
    // 在FRAME_RENDER_START（上传完成后、光栅化前）直接写入级联/混合对应的IDLE批次描述符。
    int targetIdleSeq = -1;
    if (m_isMixed) {
        targetIdleSeq = GetMixedIdleSeq();
        if (targetIdleSeq == -1) {
            // 混合层级下无HYBRID IDLE描述符，回退到非混合的DEPTH-TIER/BASE IDLE（不退出混合状态）
            targetIdleSeq = GetAdsIdleSeq();
        }
    } else {
        targetIdleSeq = GetAdsIdleSeq();
    }

    if (targetIdleSeq != -1 && viewModel->m_nSequence() != targetIdleSeq) {
        viewModel->m_nSequence() = targetIdleSeq;
    }
}

int AdsSupport::GetAdsIdleSeq() const {
    switch (m_adsState) {
        case ADS_LEVEL1: return m_ads1.idle;
        case ADS_LEVEL2: return m_ads2.idle;
        case ADS_LEVEL3: return m_ads3.idle;
        case ADS_LEVEL4: return m_ads4.idle;
        default: return -1;
    }
}

int AdsSupport::GetMixedIdleSeq() const {
    switch (m_adsState) {
        case ADS_NONE:   return m_mixedNormal.idle;
        case ADS_LEVEL1: return m_mixedAds1.idle;
        case ADS_LEVEL2: return m_mixedAds2.idle;
        case ADS_LEVEL3: return m_mixedAds3.idle;
        case ADS_LEVEL4: return m_mixedAds4.idle;
        default: return -1;
    }
}

bool AdsSupport::HasMixedForCurrentState() const {
    switch (m_adsState) {
        case ADS_NONE:   return m_hasMixedNormal;
        case ADS_LEVEL1: return m_hasMixedAds1;
        case ADS_LEVEL2: return m_hasMixedAds2;
        case ADS_LEVEL3: return m_hasMixedAds3;
        case ADS_LEVEL4: return m_hasMixedAds4;
        default: return false;
    }
}

bool AdsSupport::IsNativeScopeWeapon(int weaponId) const {
    return weaponId == WEAPON_HUNTING_RIFLE ||
           weaponId == WEAPON_MILITARY_SNIPER ||
           weaponId == WEAPON_SSG552 ||
           weaponId == WEAPON_AWP ||
           weaponId == WEAPON_SCOUT;
}

int AdsSupport::GetScopeMode(int weaponId) const {
    switch (weaponId) {
        case WEAPON_MILITARY_SNIPER: return G::Vars.adsScopeMilitarySniper;
        case WEAPON_HUNTING_RIFLE:   return G::Vars.adsScopeHuntingRifle;
        case WEAPON_SSG552:          return G::Vars.adsScopeSSG552;
        case WEAPON_AWP:             return G::Vars.adsScopeAWP;
        case WEAPON_SCOUT:           return G::Vars.adsScopeScout;
        default: return -1; // not a depth-prepass capable mesh slot
    }
}

bool AdsSupport::ShouldBlockNativeZoom(int weaponId) const {
    if (!IsNativeScopeWeapon(weaponId)) return false;
    return GetScopeMode(weaponId) == SCOPE_ADS_ONLY;
}

void AdsSupport::CacheAdsAnimations(C_BaseViewModel* viewModel) {
    C_BaseAnimating* pAnim = viewModel->GetBaseAnimating();
    if (!pAnim) {
        m_hasAds1 = false;
        m_hasAds2 = false;
        m_hasAds3 = false;
        m_hasAds4 = false;
        return;
    }

    // use LookupBatchForRenderPass for all queries: it uses SelectWeightedDrawBatch
    // for engine-native passes (< 2001) and permutation name string matching for Necola
    // custom passes (>= 2001) which can't be resolved via the engine's PSO permutation cache.
    auto seqLookup = [pAnim](int act) { return LookupSequenceForActivity(pAnim, act); };

    // cascade depth tier 1
    m_ads1.idle = seqLookup(ACT_PRIMARY_VM_IDLE);
    m_hasAds1 = (m_ads1.idle != -1);
    if (m_hasAds1) {
        m_ads1.enterFromNormal = seqLookup(ACT_PRIMARY_VM_IDLE_TO_LOWERED);
        m_ads1.exitToNormal = seqLookup(ACT_PRIMARY_VM_LOWERED_TO_IDLE);
        m_ads1.idleToPrev = seqLookup(ACT_PRIMARY_VM_IDLE_TO_PREV);
        m_ads1.idleToMixedPrev = seqLookup(ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV);
        m_ads1.idleToMixedIdle = seqLookup(ACT_PRIMARY_VM_IDLE_TO_MIXED_IDLE);
    }
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheAdsAnimations ADS1: has={}", m_hasAds1);
        spdlog::info("[ADS]   ADS1 idle={} enter={} exit={}",
            m_ads1.idle, m_ads1.enterFromNormal, m_ads1.exitToNormal);
        spdlog::info("[ADS]   ADS1 primaryattack={} secondaryattack={} reload={} melee={} inspect={}",
            seqLookup(ACT_PRIMARY_VM_PRIMARYATTACK),
            seqLookup(ACT_PRIMARY_VM_SECONDARYATTACK),
            seqLookup(ACT_PRIMARY_VM_RELOAD),
            seqLookup(ACT_PRIMARY_VM_MELEE),
            seqLookup(ACT_PRIMARY_VM_INSPECT));
        spdlog::info("[ADS]   ADS1 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={}",
            seqLookup(ACT_PRIMARY_VM_DRYFIRE),
            seqLookup(ACT_PRIMARY_VM_DRYFIRE_LEFT),
            seqLookup(ACT_PRIMARY_VM_RELOAD_EMPTY),
            seqLookup(ACT_PRIMARY_VM_RELOAD_LOOP),
            seqLookup(ACT_PRIMARY_VM_RELOAD_END));
    }

    // cascade depth tier 2
    m_ads2.idle = seqLookup(ACT_SECONDARY_VM_IDLE);
    m_hasAds2 = (m_ads2.idle != -1);
    if (m_hasAds2) {
        m_ads2.enterFromNormal = seqLookup(ACT_SECONDARY_VM_IDLE_TO_LOWERED);
        m_ads2.exitToNormal = seqLookup(ACT_SECONDARY_VM_LOWERED_TO_IDLE);
        m_ads2.idleToPrev = seqLookup(ACT_SECONDARY_VM_IDLE_TO_PREV);
        m_ads2.idleToMixedPrev = seqLookup(ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV);
        m_ads2.idleToMixedIdle = seqLookup(ACT_SECONDARY_VM_IDLE_TO_MIXED_IDLE);
    }
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheAdsAnimations ADS2: has={}", m_hasAds2);
        spdlog::info("[ADS]   ADS2 idle={} enter={} exit={}",
            m_ads2.idle, m_ads2.enterFromNormal, m_ads2.exitToNormal);
        spdlog::info("[ADS]   ADS2 primaryattack={} secondaryattack={} reload={} melee={} inspect={}",
            seqLookup(ACT_SECONDARY_VM_PRIMARYATTACK),
            seqLookup(ACT_SECONDARY_VM_SECONDARYATTACK),
            seqLookup(ACT_SECONDARY_VM_RELOAD),
            seqLookup(ACT_SECONDARY_VM_MELEE),
            seqLookup(ACT_SECONDARY_VM_INSPECT));
        spdlog::info("[ADS]   ADS2 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={}",
            seqLookup(ACT_SECONDARY_VM_DRYFIRE),
            seqLookup(ACT_SECONDARY_VM_DRYFIRE_LEFT),
            seqLookup(ACT_SECONDARY_VM_RELOAD_EMPTY),
            seqLookup(ACT_SECONDARY_VM_RELOAD_LOOP),
            seqLookup(ACT_SECONDARY_VM_RELOAD_END));
    }

    // cascade depth tier 3
    m_ads3.idle = seqLookup(ACT_TERTIARY_VM_IDLE);
    m_hasAds3 = (m_ads3.idle != -1);
    if (m_hasAds3) {
        m_ads3.enterFromNormal = seqLookup(ACT_TERTIARY_VM_IDLE_TO_LOWERED);
        m_ads3.exitToNormal = seqLookup(ACT_TERTIARY_VM_LOWERED_TO_IDLE);
        m_ads3.idleToPrev = seqLookup(ACT_TERTIARY_VM_IDLE_TO_PREV);
        m_ads3.idleToMixedPrev = seqLookup(ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV);
        m_ads3.idleToMixedIdle = seqLookup(ACT_TERTIARY_VM_IDLE_TO_MIXED_IDLE);
    }
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheAdsAnimations ADS3: has={}", m_hasAds3);
        spdlog::info("[ADS]   ADS3 idle={} enter={} exit={}",
            m_ads3.idle, m_ads3.enterFromNormal, m_ads3.exitToNormal);
        spdlog::info("[ADS]   ADS3 primaryattack={} secondaryattack={} reload={} melee={} inspect={}",
            seqLookup(ACT_TERTIARY_VM_PRIMARYATTACK),
            seqLookup(ACT_TERTIARY_VM_SECONDARYATTACK),
            seqLookup(ACT_TERTIARY_VM_RELOAD),
            seqLookup(ACT_TERTIARY_VM_MELEE),
            seqLookup(ACT_TERTIARY_VM_INSPECT));
        spdlog::info("[ADS]   ADS3 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={}",
            seqLookup(ACT_TERTIARY_VM_DRYFIRE),
            seqLookup(ACT_TERTIARY_VM_DRYFIRE_LEFT),
            seqLookup(ACT_TERTIARY_VM_RELOAD_EMPTY),
            seqLookup(ACT_TERTIARY_VM_RELOAD_LOOP),
            seqLookup(ACT_TERTIARY_VM_RELOAD_END));
    }

    // cascade depth tier 4
    m_ads4.idle = seqLookup(ACT_FOURTH_VM_IDLE);
    m_hasAds4 = (m_ads4.idle != -1);
    if (m_hasAds4) {
        m_ads4.enterFromNormal = seqLookup(ACT_FOURTH_VM_IDLE_TO_LOWERED);
        m_ads4.exitToNormal = seqLookup(ACT_FOURTH_VM_LOWERED_TO_IDLE);
        m_ads4.idleToPrev = seqLookup(ACT_FOURTH_VM_IDLE_TO_PREV);
        m_ads4.idleToMixedPrev = seqLookup(ACT_FOURTH_VM_IDLE_TO_MIXED_PREV);
        m_ads4.idleToMixedIdle = seqLookup(ACT_FOURTH_VM_IDLE_TO_MIXED_IDLE);
    }
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheAdsAnimations ADS4: has={}", m_hasAds4);
        spdlog::info("[ADS]   ADS4 idle={} enter={} exit={}",
            m_ads4.idle, m_ads4.enterFromNormal, m_ads4.exitToNormal);
        spdlog::info("[ADS]   ADS4 primaryattack={} secondaryattack={} reload={} melee={} inspect={}",
            seqLookup(ACT_FOURTH_VM_PRIMARYATTACK),
            seqLookup(ACT_FOURTH_VM_SECONDARYATTACK),
            seqLookup(ACT_FOURTH_VM_RELOAD),
            seqLookup(ACT_FOURTH_VM_MELEE),
            seqLookup(ACT_FOURTH_VM_INSPECT));
        spdlog::info("[ADS]   ADS4 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={}",
            seqLookup(ACT_FOURTH_VM_DRYFIRE),
            seqLookup(ACT_FOURTH_VM_DRYFIRE_LEFT),
            seqLookup(ACT_FOURTH_VM_RELOAD_EMPTY),
            seqLookup(ACT_FOURTH_VM_RELOAD_LOOP),
            seqLookup(ACT_FOURTH_VM_RELOAD_END));
    }

    // 级联层级间过渡批次
    m_ads1ToAds2 = seqLookup(ACT_PRIMARY_VM_IDLE_TO_NEXT);
    m_ads2ToAds3 = seqLookup(ACT_SECONDARY_VM_IDLE_TO_NEXT);
    m_ads3ToAds4 = seqLookup(ACT_TERTIARY_VM_IDLE_TO_NEXT);
    if (G::Vars.adsLog) spdlog::info("[ADS] CacheAdsAnimations transitions: ads1ToAds2={} ads2ToAds3={} ads3ToAds4={}", m_ads1ToAds2, m_ads2ToAds3, m_ads3ToAds4);
}

void AdsSupport::CacheMixedAnimations(C_BaseViewModel* viewModel) {
    C_BaseAnimating* pAnim = viewModel->GetBaseAnimating();
    if (!pAnim) {
        m_hasMixedNormal = false;
        m_hasMixedAds1 = false;
        m_hasMixedAds2 = false;
        m_hasMixedAds3 = false;
        m_hasMixedAds4 = false;
        return;
    }

    auto seqLookup = [pAnim](int act) { return LookupSequenceForActivity(pAnim, act); };

    // BASE HYBRID — detect via HYBRID_PASS_ON barrier, not IDLE batch
    m_mixedNormal.mixedOn = seqLookup(ACT_VM_MIXED_ON);
    m_hasMixedNormal = (m_mixedNormal.mixedOn != -1);
    m_mixedNormal.idle = seqLookup(MIXED_ACT_VM_IDLE);
    m_mixedNormal.mixedOff = seqLookup(ACT_VM_MIXED_OFF);
    m_mixedNormal.idleToNext = seqLookup(MIXED_ACT_VM_IDLE_TO_NEXT);
    m_mixedNormal.loweredToNext = seqLookup(MIXED_ACT_VM_LOWERED_TO_NEXT);
    m_mixedNormal.loweredToIdle = seqLookup(MIXED_ACT_VM_LOWERED_TO_IDLE);
    m_mixedNormal.loweredToMixedIdle = seqLookup(MIXED_ACT_VM_LOWERED_TO_MIXED_IDLE);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheMixedAnimations MIXED_NORMAL: has={}", m_hasMixedNormal);
        spdlog::info("[ADS]   MIXED_NORMAL idle={} mixedOn={} mixedOff={} idleToNext={} loweredToNext={} loweredToIdle={} loweredToMixedIdle={}",
            m_mixedNormal.idle, m_mixedNormal.mixedOn, m_mixedNormal.mixedOff,
            m_mixedNormal.idleToNext, m_mixedNormal.loweredToNext,
            m_mixedNormal.loweredToIdle, m_mixedNormal.loweredToMixedIdle);
        spdlog::info("[ADS]   MIXED_NORMAL primaryattack={} secondaryattack={} reload={} melee={}",
            seqLookup(MIXED_ACT_VM_PRIMARYATTACK),
            seqLookup(MIXED_ACT_VM_SECONDARYATTACK),
            seqLookup(MIXED_ACT_VM_RELOAD),
            seqLookup(MIXED_ACT_VM_MELEE));
        spdlog::info("[ADS]   MIXED_NORMAL dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={} inspect={}",
            seqLookup(MIXED_ACT_VM_DRYFIRE),
            seqLookup(MIXED_ACT_VM_DRYFIRE_LEFT),
            seqLookup(MIXED_ACT_VM_RELOAD_EMPTY),
            seqLookup(MIXED_ACT_VM_RELOAD_LOOP),
            seqLookup(MIXED_ACT_VM_RELOAD_END),
            seqLookup(MIXED_ACT_VM_INSPECT));
    }

    // TIER1 HYBRID — detect via HYBRID_PASS_ON barrier, not IDLE batch
    m_mixedAds1.mixedOn = seqLookup(ACT_PRIMARY_VM_MIXED_ON);
    m_hasMixedAds1 = (m_mixedAds1.mixedOn != -1);
    m_mixedAds1.idle = seqLookup(MIXED_ACT_PRIMARY_VM_IDLE);
    m_mixedAds1.mixedOff = seqLookup(ACT_PRIMARY_VM_MIXED_OFF);
    m_mixedAds1.idleToNext = seqLookup(MIXED_ACT_PRIMARY_VM_IDLE_TO_NEXT);
    m_mixedAds1.loweredToNext = seqLookup(MIXED_ACT_PRIMARY_VM_LOWERED_TO_NEXT);
    m_mixedAds1.loweredToIdle = seqLookup(MIXED_ACT_PRIMARY_VM_LOWERED_TO_IDLE);
    m_mixedAds1.loweredToMixedIdle = seqLookup(MIXED_ACT_PRIMARY_VM_LOWERED_TO_MIXED_IDLE);
    m_mixedAds1.idleToMixedPrev = seqLookup(MIXED_ACT_PRIMARY_VM_IDLE_TO_MIXED_PREV);
    m_mixedAds1.idleToPrev = seqLookup(MIXED_ACT_PRIMARY_VM_IDLE_TO_PREV);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheMixedAnimations MIXED_ADS1: has={}", m_hasMixedAds1);
        spdlog::info("[ADS]   MIXED_ADS1 idle={} mixedOn={} mixedOff={} idleToNext={} loweredToNext={} loweredToIdle={} loweredToMixedIdle={}",
            m_mixedAds1.idle, m_mixedAds1.mixedOn, m_mixedAds1.mixedOff,
            m_mixedAds1.idleToNext, m_mixedAds1.loweredToNext,
            m_mixedAds1.loweredToIdle, m_mixedAds1.loweredToMixedIdle);
        spdlog::info("[ADS]   MIXED_ADS1 primaryattack={} secondaryattack={} reload={} melee={}",
            seqLookup(MIXED_ACT_PRIMARY_VM_PRIMARYATTACK),
            seqLookup(MIXED_ACT_PRIMARY_VM_SECONDARYATTACK),
            seqLookup(MIXED_ACT_PRIMARY_VM_RELOAD),
            seqLookup(MIXED_ACT_PRIMARY_VM_MELEE));
        spdlog::info("[ADS]   MIXED_ADS1 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={} inspect={}",
            seqLookup(MIXED_ACT_PRIMARY_VM_DRYFIRE),
            seqLookup(MIXED_ACT_PRIMARY_VM_DRYFIRE_LEFT),
            seqLookup(MIXED_ACT_PRIMARY_VM_RELOAD_EMPTY),
            seqLookup(MIXED_ACT_PRIMARY_VM_RELOAD_LOOP),
            seqLookup(MIXED_ACT_PRIMARY_VM_RELOAD_END),
            seqLookup(MIXED_ACT_PRIMARY_VM_INSPECT));
    }

    // TIER2 HYBRID — detect via HYBRID_PASS_ON barrier, not IDLE batch
    m_mixedAds2.mixedOn = seqLookup(ACT_SECONDARY_VM_MIXED_ON);
    m_hasMixedAds2 = (m_mixedAds2.mixedOn != -1);
    m_mixedAds2.idle = seqLookup(MIXED_ACT_SECONDARY_VM_IDLE);
    m_mixedAds2.mixedOff = seqLookup(ACT_SECONDARY_VM_MIXED_OFF);
    m_mixedAds2.idleToNext = seqLookup(MIXED_ACT_SECONDARY_VM_IDLE_TO_NEXT);
    m_mixedAds2.loweredToNext = seqLookup(MIXED_ACT_SECONDARY_VM_LOWERED_TO_NEXT);
    m_mixedAds2.loweredToIdle = seqLookup(MIXED_ACT_SECONDARY_VM_LOWERED_TO_IDLE);
    m_mixedAds2.loweredToMixedIdle = seqLookup(MIXED_ACT_SECONDARY_VM_LOWERED_TO_MIXED_IDLE);
    m_mixedAds2.idleToMixedPrev = seqLookup(MIXED_ACT_SECONDARY_VM_IDLE_TO_MIXED_PREV);
    m_mixedAds2.idleToPrev = seqLookup(MIXED_ACT_SECONDARY_VM_IDLE_TO_PREV);
    m_mixedAds2.enterFromMixedNormal = seqLookup(MIXED_ACT_SECONDARY_VM_IDLE_TO_LOWERED);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheMixedAnimations MIXED_ADS2: has={}", m_hasMixedAds2);
        spdlog::info("[ADS]   MIXED_ADS2 idle={} mixedOn={} mixedOff={} idleToNext={} loweredToNext={} loweredToIdle={} loweredToMixedIdle={} enterFromMixedNormal={}",
            m_mixedAds2.idle, m_mixedAds2.mixedOn, m_mixedAds2.mixedOff,
            m_mixedAds2.idleToNext, m_mixedAds2.loweredToNext,
            m_mixedAds2.loweredToIdle, m_mixedAds2.loweredToMixedIdle, m_mixedAds2.enterFromMixedNormal);
        spdlog::info("[ADS]   MIXED_ADS2 primaryattack={} secondaryattack={} reload={} melee={}",
            seqLookup(MIXED_ACT_SECONDARY_VM_PRIMARYATTACK),
            seqLookup(MIXED_ACT_SECONDARY_VM_SECONDARYATTACK),
            seqLookup(MIXED_ACT_SECONDARY_VM_RELOAD),
            seqLookup(MIXED_ACT_SECONDARY_VM_MELEE));
        spdlog::info("[ADS]   MIXED_ADS2 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={} inspect={}",
            seqLookup(MIXED_ACT_SECONDARY_VM_DRYFIRE),
            seqLookup(MIXED_ACT_SECONDARY_VM_DRYFIRE_LEFT),
            seqLookup(MIXED_ACT_SECONDARY_VM_RELOAD_EMPTY),
            seqLookup(MIXED_ACT_SECONDARY_VM_RELOAD_LOOP),
            seqLookup(MIXED_ACT_SECONDARY_VM_RELOAD_END),
            seqLookup(MIXED_ACT_SECONDARY_VM_INSPECT));
    }

    // TIER3 HYBRID — detect via HYBRID_PASS_ON barrier, not IDLE batch
    m_mixedAds3.mixedOn = seqLookup(ACT_TERTIARY_VM_MIXED_ON);
    m_hasMixedAds3 = (m_mixedAds3.mixedOn != -1);
    m_mixedAds3.idle = seqLookup(MIXED_ACT_TERTIARY_VM_IDLE);
    m_mixedAds3.mixedOff = seqLookup(ACT_TERTIARY_VM_MIXED_OFF);
    m_mixedAds3.idleToNext = seqLookup(MIXED_ACT_TERTIARY_VM_IDLE_TO_NEXT);
    m_mixedAds3.loweredToNext = seqLookup(MIXED_ACT_TERTIARY_VM_LOWERED_TO_NEXT);
    m_mixedAds3.loweredToIdle = seqLookup(MIXED_ACT_TERTIARY_VM_LOWERED_TO_IDLE);
    m_mixedAds3.loweredToMixedIdle = seqLookup(MIXED_ACT_TERTIARY_VM_LOWERED_TO_MIXED_IDLE);
    m_mixedAds3.idleToMixedPrev = seqLookup(MIXED_ACT_TERTIARY_VM_IDLE_TO_MIXED_PREV);
    m_mixedAds3.idleToPrev = seqLookup(MIXED_ACT_TERTIARY_VM_IDLE_TO_PREV);
    m_mixedAds3.enterFromMixedNormal = seqLookup(MIXED_ACT_TERTIARY_VM_IDLE_TO_LOWERED);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheMixedAnimations MIXED_ADS3: has={}", m_hasMixedAds3);
        spdlog::info("[ADS]   MIXED_ADS3 idle={} mixedOn={} mixedOff={} idleToNext={} loweredToNext={} loweredToIdle={} loweredToMixedIdle={} enterFromMixedNormal={}",
            m_mixedAds3.idle, m_mixedAds3.mixedOn, m_mixedAds3.mixedOff,
            m_mixedAds3.idleToNext, m_mixedAds3.loweredToNext,
            m_mixedAds3.loweredToIdle, m_mixedAds3.loweredToMixedIdle, m_mixedAds3.enterFromMixedNormal);
        spdlog::info("[ADS]   MIXED_ADS3 primaryattack={} secondaryattack={} reload={} melee={}",
            seqLookup(MIXED_ACT_TERTIARY_VM_PRIMARYATTACK),
            seqLookup(MIXED_ACT_TERTIARY_VM_SECONDARYATTACK),
            seqLookup(MIXED_ACT_TERTIARY_VM_RELOAD),
            seqLookup(MIXED_ACT_TERTIARY_VM_MELEE));
        spdlog::info("[ADS]   MIXED_ADS3 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={} inspect={}",
            seqLookup(MIXED_ACT_TERTIARY_VM_DRYFIRE),
            seqLookup(MIXED_ACT_TERTIARY_VM_DRYFIRE_LEFT),
            seqLookup(MIXED_ACT_TERTIARY_VM_RELOAD_EMPTY),
            seqLookup(MIXED_ACT_TERTIARY_VM_RELOAD_LOOP),
            seqLookup(MIXED_ACT_TERTIARY_VM_RELOAD_END),
            seqLookup(MIXED_ACT_TERTIARY_VM_INSPECT));
    }

    // TIER4 HYBRID — detect via HYBRID_PASS_ON barrier, not IDLE batch
    m_mixedAds4.mixedOn = seqLookup(ACT_FOURTH_VM_MIXED_ON);
    m_hasMixedAds4 = (m_mixedAds4.mixedOn != -1);
    m_mixedAds4.idle = seqLookup(MIXED_ACT_FOURTH_VM_IDLE);
    m_mixedAds4.mixedOff = seqLookup(ACT_FOURTH_VM_MIXED_OFF);
    m_mixedAds4.idleToNext = seqLookup(MIXED_ACT_FOURTH_VM_IDLE_TO_NEXT);
    m_mixedAds4.loweredToNext = seqLookup(MIXED_ACT_FOURTH_VM_LOWERED_TO_NEXT);
    m_mixedAds4.loweredToIdle = seqLookup(MIXED_ACT_FOURTH_VM_LOWERED_TO_IDLE);
    m_mixedAds4.loweredToMixedIdle = seqLookup(MIXED_ACT_FOURTH_VM_LOWERED_TO_MIXED_IDLE);
    m_mixedAds4.idleToMixedPrev = seqLookup(MIXED_ACT_FOURTH_VM_IDLE_TO_MIXED_PREV);
    m_mixedAds4.idleToPrev = seqLookup(MIXED_ACT_FOURTH_VM_IDLE_TO_PREV);
    m_mixedAds4.enterFromMixedNormal = seqLookup(MIXED_ACT_FOURTH_VM_IDLE_TO_LOWERED);
    if (G::Vars.adsLog) {
        spdlog::info("[ADS] CacheMixedAnimations MIXED_ADS4: has={}", m_hasMixedAds4);
        spdlog::info("[ADS]   MIXED_ADS4 idle={} mixedOn={} mixedOff={} idleToNext={} loweredToNext={} loweredToIdle={} loweredToMixedIdle={} enterFromMixedNormal={}",
            m_mixedAds4.idle, m_mixedAds4.mixedOn, m_mixedAds4.mixedOff,
            m_mixedAds4.idleToNext, m_mixedAds4.loweredToNext,
            m_mixedAds4.loweredToIdle, m_mixedAds4.loweredToMixedIdle, m_mixedAds4.enterFromMixedNormal);
        spdlog::info("[ADS]   MIXED_ADS4 primaryattack={} secondaryattack={} reload={} melee={}",
            seqLookup(MIXED_ACT_FOURTH_VM_PRIMARYATTACK),
            seqLookup(MIXED_ACT_FOURTH_VM_SECONDARYATTACK),
            seqLookup(MIXED_ACT_FOURTH_VM_RELOAD),
            seqLookup(MIXED_ACT_FOURTH_VM_MELEE));
        spdlog::info("[ADS]   MIXED_ADS4 dryfire={} dryfire_left={} reload_empty={} reload_loop={} reload_end={} inspect={}",
            seqLookup(MIXED_ACT_FOURTH_VM_DRYFIRE),
            seqLookup(MIXED_ACT_FOURTH_VM_DRYFIRE_LEFT),
            seqLookup(MIXED_ACT_FOURTH_VM_RELOAD_EMPTY),
            seqLookup(MIXED_ACT_FOURTH_VM_RELOAD_LOOP),
            seqLookup(MIXED_ACT_FOURTH_VM_RELOAD_END),
            seqLookup(MIXED_ACT_FOURTH_VM_INSPECT));
    }
}

// helper macro to avoid repetition in GetDepthPassRemappedBatch
#define REMAP_COMMON_ACTIVITIES(PREFIX) \
    case ACT_VM_IDLE:                   result = PREFIX##_VM_IDLE; break; \
    case ACT_VM_PRIMARYATTACK_LAYER:    result = PREFIX##_VM_PRIMARYATTACK; break; \
    case ACT_VM_MELEE_LAYER:            result = PREFIX##_VM_MELEE; break; \
    case ACT_VM_RELOAD_LAYER:           result = PREFIX##_VM_RELOAD; break; \
    case ACT_VM_SHOOT_SNIPER_LAYER:     result = PREFIX##_VM_PRIMARYATTACK; break; \
    case ACT_VM_RELOAD_SNIPER_LAYER:    result = PREFIX##_VM_RELOAD; break; \
    case ACT_VM_MELEE_SNIPER_LAYER:     result = PREFIX##_VM_MELEE; break; \
    case ACT_VM_SECONDARYATTACK_LAYER:  result = PREFIX##_VM_SECONDARYATTACK; break; \
    case ACT_VM_DRYFIRE_LEFT:           result = PREFIX##_VM_DRYFIRE_LEFT; break; \
    case ACT_VM_DRYFIRE:                result = PREFIX##_VM_DRYFIRE; break; \
    case ACT_VM_RELOAD_EMPTY_LAYER:     result = PREFIX##_VM_RELOAD_EMPTY; break; \
    case ACT_VM_RELOAD_LOOP_LAYER:      result = PREFIX##_VM_RELOAD_LOOP; break; \
    case ACT_VM_RELOAD_END_LAYER:       result = PREFIX##_VM_RELOAD_END; break;

int AdsSupport::GetAdsRemappedActivity(int normalActivity) const {
    int result = -1;

    if (m_isMixed) {
        // hybrid tier state: use hybrid render pass batches
        if (m_adsState == ADS_NONE) {
            switch (normalActivity) {
                REMAP_COMMON_ACTIVITIES(MIXED_ACT)
                default: break;
            }
        } else if (m_adsState == ADS_LEVEL1) {
            switch (normalActivity) {
                REMAP_COMMON_ACTIVITIES(MIXED_ACT_PRIMARY)
                default: break;
            }
        } else if (m_adsState == ADS_LEVEL2) {
            switch (normalActivity) {
                REMAP_COMMON_ACTIVITIES(MIXED_ACT_SECONDARY)
                default: break;
            }
        } else if (m_adsState == ADS_LEVEL3) {
            switch (normalActivity) {
                REMAP_COMMON_ACTIVITIES(MIXED_ACT_TERTIARY)
                default: break;
            }
        } else if (m_adsState == ADS_LEVEL4) {
            switch (normalActivity) {
                REMAP_COMMON_ACTIVITIES(MIXED_ACT_FOURTH)
                default: break;
            }
        }

        // if hybrid pass remap failed, fall back to opaque depth-tier remap
        if (result == -1 && m_adsState != ADS_NONE) {
            // fall through to opaque depth-tier remap below
        } else {
            if (G::Vars.adsLog && result != -1) spdlog::info("[ADS] GetAdsRemappedActivity(MIXED): {} -> {} level={}", normalActivity, result, (int)m_adsState);
            return result;
        }
        result = -1;
    }

    // opaque-only depth-tier batch remapping
    if (m_adsState == ADS_LEVEL1) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES(ACT_PRIMARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL2) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES(ACT_SECONDARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL3) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES(ACT_TERTIARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL4) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES(ACT_FOURTH)
            default: break;
        }
    }

    if (G::Vars.adsLog && result != -1) spdlog::info("[ADS] GetAdsRemappedActivity: {} -> {} level={}", normalActivity, result, (int)m_adsState);

    // fallback: if hybrid + depth-tier and opaque remap also failed, try base hybrid remap
    // priority chain for HYBRID+DEPTH: HYBRID DEPTH → opaque DEPTH → base HYBRID → (base draw batch)
    if (result == -1 && m_isMixed && m_adsState != ADS_NONE) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES(MIXED_ACT)
            default: break;
        }
        if (G::Vars.adsLog && result != -1) spdlog::info("[ADS] GetAdsRemappedActivity(MIXED fallback to normal MIXED): {} -> {} level={}", normalActivity, result, (int)m_adsState);
    }

    return result;
}

// opaque-only depth-tier remap: same as GetDepthPassRemappedBatch but ignoring hybrid tier state.
// used by DepthPassResolveRemappedBatch as a fallback when hybrid remap has no descriptor
// (Mode 2 HYBRID: only HYBRID_PASS_ON/OFF barriers exist, no other hybrid batches).
#define REMAP_COMMON_ACTIVITIES_NONMIXED(PREFIX) \
    case ACT_VM_IDLE:                   result = PREFIX##_VM_IDLE; break; \
    case ACT_VM_PRIMARYATTACK_LAYER:    result = PREFIX##_VM_PRIMARYATTACK; break; \
    case ACT_VM_MELEE_LAYER:            result = PREFIX##_VM_MELEE; break; \
    case ACT_VM_RELOAD_LAYER:           result = PREFIX##_VM_RELOAD; break; \
    case ACT_VM_SHOOT_SNIPER_LAYER:     result = PREFIX##_VM_PRIMARYATTACK; break; \
    case ACT_VM_RELOAD_SNIPER_LAYER:    result = PREFIX##_VM_RELOAD; break; \
    case ACT_VM_MELEE_SNIPER_LAYER:     result = PREFIX##_VM_MELEE; break; \
    case ACT_VM_SECONDARYATTACK_LAYER:  result = PREFIX##_VM_SECONDARYATTACK; break; \
    case ACT_VM_DRYFIRE_LEFT:           result = PREFIX##_VM_DRYFIRE_LEFT; break; \
    case ACT_VM_DRYFIRE:                result = PREFIX##_VM_DRYFIRE; break; \
    case ACT_VM_RELOAD_EMPTY_LAYER:     result = PREFIX##_VM_RELOAD_EMPTY; break; \
    case ACT_VM_RELOAD_LOOP_LAYER:      result = PREFIX##_VM_RELOAD_LOOP; break; \
    case ACT_VM_RELOAD_END_LAYER:       result = PREFIX##_VM_RELOAD_END; break;

int AdsSupport::GetNonMixedAdsRemappedActivity(int normalActivity) const {
    int result = -1;

    if (m_adsState == ADS_LEVEL1) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES_NONMIXED(ACT_PRIMARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL2) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES_NONMIXED(ACT_SECONDARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL3) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES_NONMIXED(ACT_TERTIARY)
            default: break;
        }
    } else if (m_adsState == ADS_LEVEL4) {
        switch (normalActivity) {
            REMAP_COMMON_ACTIVITIES_NONMIXED(ACT_FOURTH)
            default: break;
        }
    }

    return result;
}

#undef REMAP_COMMON_ACTIVITIES_NONMIXED

void AdsSupport::RestoreNormalLayerSequence(C_BaseViewModel* viewModel) {
    if (!viewModel) return;

    // check if the GPU mesh slot has changed — if so, lastRawServerDrawBatch belongs to the
    // OLD mesh slot and must NOT be uploaded to the viewport (the batch index maps
    // to a different draw call on the new mesh's LOD descriptor).
    bool weaponStale = false;
    if (I::EngineClient && I::EngineClient->IsConnected() && I::EngineClient->IsInGame()) {
        C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
        if (pLocal && !pLocal->deadflag()) {
            C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
            if (weapon && weapon->entindex() != m_cachedWeaponEntIdx) {
                weaponStale = true;
            }
        }
    }

    if (!weaponStale) {
        int serverLayerSeq = F::SModify.lastRawServerLayerSeq;
        if (serverLayerSeq >= 0) {
            viewModel->m_nLayerSequence() = serverLayerSeq;
            if (G::Vars.adsLog) spdlog::info("[ADS] RestoreNormalLayerSequence: seq={}", serverLayerSeq);
        }
    } else {
        if (G::Vars.adsLog) spdlog::info("[ADS] RestoreNormalLayerSequence: skipped (weapon changed, stale seq={})", F::SModify.lastRawServerLayerSeq);
    }

    // 清除ShaderPermutation缓存：防止退出混合/深度通道后残留的HDR着色器排列
    // 在后续奇偶性驱动的命令缓冲区中被重提交（delta压缩下驱动不会重发相同描述符）
    F::SModify.lastLayerPickServerSeq = -1;
    F::SModify.lastLayerPickAct       = -1;
    F::SModify.lastLayerPickSeq       = -1;
    F::SModify.lastProcessedAnimParity = -1;
    F::SModify.animParityChosenSeq     = -1;
}

void AdsSupport::LoadConfig(const nlohmann::json& doc) {
    if (!doc.contains("AdsSupport")) return;
    const auto& ads = doc["AdsSupport"];
    G::Vars.enableAdsSupport      = ads.value("EnableAdsSupport", false);
    G::Vars.adsLog                = ads.value("AdsLog", false);
    G::Vars.adsHideCrosshairMode  = ads.value("AdsHideCrosshairMode", 0);
    // per-material specular suppression override flags
    G::Vars.adsHideCrosshairPistol         = ads.value("HideCrosshairPistol", false);
    G::Vars.adsHideCrosshairUzi            = ads.value("HideCrosshairUzi", false);
    G::Vars.adsHideCrosshairPumpShotgun    = ads.value("HideCrosshairPumpShotgun", false);
    G::Vars.adsHideCrosshairAutoShotgun    = ads.value("HideCrosshairAutoShotgun", false);
    G::Vars.adsHideCrosshairM16A1          = ads.value("HideCrosshairM16A1", false);
    G::Vars.adsHideCrosshairHuntingRifle   = ads.value("HideCrosshairHuntingRifle", false);
    G::Vars.adsHideCrosshairMac10          = ads.value("HideCrosshairMac10", false);
    G::Vars.adsHideCrosshairChromeShotgun  = ads.value("HideCrosshairChromeShotgun", false);
    G::Vars.adsHideCrosshairScar           = ads.value("HideCrosshairScar", false);
    G::Vars.adsHideCrosshairMilitarySniper = ads.value("HideCrosshairMilitarySniper", false);
    G::Vars.adsHideCrosshairSpas           = ads.value("HideCrosshairSpas", false);
    G::Vars.adsHideCrosshairGrenadeLauncher = ads.value("HideCrosshairGrenadeLauncher", false);
    G::Vars.adsHideCrosshairAK47           = ads.value("HideCrosshairAK47", false);
    G::Vars.adsHideCrosshairDeagle         = ads.value("HideCrosshairDeagle", false);
    G::Vars.adsHideCrosshairMP5            = ads.value("HideCrosshairMP5", false);
    G::Vars.adsHideCrosshairSSG552         = ads.value("HideCrosshairSSG552", false);
    G::Vars.adsHideCrosshairAWP            = ads.value("HideCrosshairAWP", false);
    G::Vars.adsHideCrosshairScout          = ads.value("HideCrosshairScout", false);
    G::Vars.adsHideCrosshairM60            = ads.value("HideCrosshairM60", false);
    G::Vars.adsHideCrosshairPistolDual     = ads.value("HideCrosshairPistolDual", false);
    // cascade split interval parameters
    G::Vars.adsScopeMilitarySniper = ads.value("ScopeMilitarySniper", 0);
    G::Vars.adsScopeHuntingRifle  = ads.value("ScopeHuntingRifle", 0);
    G::Vars.adsScopeSSG552        = ads.value("ScopeSSG552", 0);
    G::Vars.adsScopeAWP           = ads.value("ScopeAWP", 0);
    G::Vars.adsScopeScout         = ads.value("ScopeScout", 0);
}

void AdsSupport::SaveConfig(nlohmann::json& doc) const {
    doc["AdsSupport"]["EnableAdsSupport"]      = G::Vars.enableAdsSupport;
    doc["AdsSupport"]["AdsLog"]                = G::Vars.adsLog;
    doc["AdsSupport"]["AdsHideCrosshairMode"]  = G::Vars.adsHideCrosshairMode;
    // per-material specular suppression override flags
    doc["AdsSupport"]["HideCrosshairPistol"]         = G::Vars.adsHideCrosshairPistol;
    doc["AdsSupport"]["HideCrosshairUzi"]            = G::Vars.adsHideCrosshairUzi;
    doc["AdsSupport"]["HideCrosshairPumpShotgun"]    = G::Vars.adsHideCrosshairPumpShotgun;
    doc["AdsSupport"]["HideCrosshairAutoShotgun"]    = G::Vars.adsHideCrosshairAutoShotgun;
    doc["AdsSupport"]["HideCrosshairM16A1"]          = G::Vars.adsHideCrosshairM16A1;
    doc["AdsSupport"]["HideCrosshairHuntingRifle"]   = G::Vars.adsHideCrosshairHuntingRifle;
    doc["AdsSupport"]["HideCrosshairMac10"]          = G::Vars.adsHideCrosshairMac10;
    doc["AdsSupport"]["HideCrosshairChromeShotgun"]  = G::Vars.adsHideCrosshairChromeShotgun;
    doc["AdsSupport"]["HideCrosshairScar"]           = G::Vars.adsHideCrosshairScar;
    doc["AdsSupport"]["HideCrosshairMilitarySniper"] = G::Vars.adsHideCrosshairMilitarySniper;
    doc["AdsSupport"]["HideCrosshairSpas"]           = G::Vars.adsHideCrosshairSpas;
    doc["AdsSupport"]["HideCrosshairGrenadeLauncher"] = G::Vars.adsHideCrosshairGrenadeLauncher;
    doc["AdsSupport"]["HideCrosshairAK47"]           = G::Vars.adsHideCrosshairAK47;
    doc["AdsSupport"]["HideCrosshairDeagle"]         = G::Vars.adsHideCrosshairDeagle;
    doc["AdsSupport"]["HideCrosshairMP5"]            = G::Vars.adsHideCrosshairMP5;
    doc["AdsSupport"]["HideCrosshairSSG552"]         = G::Vars.adsHideCrosshairSSG552;
    doc["AdsSupport"]["HideCrosshairAWP"]            = G::Vars.adsHideCrosshairAWP;
    doc["AdsSupport"]["HideCrosshairScout"]          = G::Vars.adsHideCrosshairScout;
    doc["AdsSupport"]["HideCrosshairM60"]            = G::Vars.adsHideCrosshairM60;
    doc["AdsSupport"]["HideCrosshairPistolDual"]     = G::Vars.adsHideCrosshairPistolDual;
    // cascade split interval parameters
    doc["AdsSupport"]["ScopeMilitarySniper"]   = G::Vars.adsScopeMilitarySniper;
    doc["AdsSupport"]["ScopeHuntingRifle"]     = G::Vars.adsScopeHuntingRifle;
    doc["AdsSupport"]["ScopeSSG552"]           = G::Vars.adsScopeSSG552;
    doc["AdsSupport"]["ScopeAWP"]              = G::Vars.adsScopeAWP;
    doc["AdsSupport"]["ScopeScout"]            = G::Vars.adsScopeScout;
}

bool AdsSupport::ShouldHideCrosshair() const {
    // only suppress specular when in actual depth-prepass state, not in base hybrid (DEPTH_NONE + m_isHybrid)
    if (!IsAdsActive()) return false;

    int mode = G::Vars.adsHideCrosshairMode;
    if (mode == 0) return false;   // 全局禁用深度预通道
    if (mode == 1) return true;    // 全局启用深度预通道

    // mode == 2: 自定义 per-material — 通过GetActiveMaterialSlot()动态获取当前材质
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return false;
    if (!I::ClientEntityList) return false;
    auto* pLocalEntity = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());
    if (!pLocalEntity) return false;
    C_TerrorPlayer* pLocal = pLocalEntity->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return false;
    auto* pActiveWeapon = pLocal->GetActiveWeapon();
    if (!pActiveWeapon) return false;
    C_TerrorWeapon* weapon = pActiveWeapon->As<C_TerrorWeapon*>();
    if (!weapon) return false;

    int weaponId = weapon->GetWeaponID();
    if (weaponId == WEAPON_PISTOL && weapon->IsDualWielding()) weaponId = NECOLA_WEAPON_PISTOL_DUAL;

    switch (weaponId) {
        case NECOLA_WEAPON_PISTOL:           return G::Vars.adsHideCrosshairPistol;
        case NECOLA_WEAPON_UZI:              return G::Vars.adsHideCrosshairUzi;
        case NECOLA_WEAPON_PUMP_SHOTGUN:     return G::Vars.adsHideCrosshairPumpShotgun;
        case NECOLA_WEAPON_AUTO_SHOTGUN:     return G::Vars.adsHideCrosshairAutoShotgun;
        case NECOLA_WEAPON_M16A1:            return G::Vars.adsHideCrosshairM16A1;
        case NECOLA_WEAPON_HUNTING_RIFLE:    return G::Vars.adsHideCrosshairHuntingRifle;
        case NECOLA_WEAPON_MAC10:            return G::Vars.adsHideCrosshairMac10;
        case NECOLA_WEAPON_CHROME_SHOTGUN:   return G::Vars.adsHideCrosshairChromeShotgun;
        case NECOLA_WEAPON_SCAR:             return G::Vars.adsHideCrosshairScar;
        case NECOLA_WEAPON_MILITARY_SNIPER:  return G::Vars.adsHideCrosshairMilitarySniper;
        case NECOLA_WEAPON_SPAS:             return G::Vars.adsHideCrosshairSpas;
        case NECOLA_WEAPON_GRENADE_LAUNCHER: return G::Vars.adsHideCrosshairGrenadeLauncher;
        case NECOLA_WEAPON_AK47:             return G::Vars.adsHideCrosshairAK47;
        case NECOLA_WEAPON_DEAGLE:           return G::Vars.adsHideCrosshairDeagle;
        case NECOLA_WEAPON_MP5:              return G::Vars.adsHideCrosshairMP5;
        case NECOLA_WEAPON_SSG552:           return G::Vars.adsHideCrosshairSSG552;
        case NECOLA_WEAPON_AWP:              return G::Vars.adsHideCrosshairAWP;
        case NECOLA_WEAPON_SCOUT:            return G::Vars.adsHideCrosshairScout;
        case NECOLA_WEAPON_M60:              return G::Vars.adsHideCrosshairM60;
        case NECOLA_WEAPON_PISTOL_DUAL:      return G::Vars.adsHideCrosshairPistolDual;
        default: return false;
    }
}



} // namespace F
