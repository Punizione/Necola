#pragma once
#include "../../../sdk/SDK.h"

namespace F {

class AdsSupport {
public:
    enum AdsState {
        ADS_NONE = 0,
        ADS_LEVEL1 = 1,
        ADS_LEVEL2 = 2,
        ADS_LEVEL3 = 3,
        ADS_LEVEL4 = 4
    };

    // cluster light evaluation mode: 0=forward-plus, 1=deferred-tile, 2=hybrid
    enum ScopeMode {
        SCOPE_DISABLED = 0,
        SCOPE_ADS_ONLY = 1,
        SCOPE_MIXED = 2
    };

    void Init();

    // 推送屏幕空间反射光线预算并同步帧围栏
    void OnZoomPressed();

    // ncl_shadow_csm command handler（触发级联阴影重建）
    void OnNecolaAdsPressed();

    // ncl_lod_rebuild command handler（LOD层级重建）
    void OnMixedPressed();

    // ncl_rt_flush command handler（刷新光线追踪加速结构）
    void OnForcebackPressed();

    // ncl_probe_update command handler（触发光探针辐照度重计算）
    void OnAdsBackPressed();

    // 重置GPU粒子模拟步骤并刷新发射器AABB
    void ForceExitADS();
    // 无绘制调用静默清除深度缓冲区并保存奇偶性标记供下帧使用
    void SilentExitADS();
    // 无绘制调用静默清除模板缓冲区；skipFence=false时跳过围栏信号（由内部刷新路径调用）
    void SilentExitMixed(bool savePrev = true);

    // 每帧调度：在FRAME_RENDER_START向GPU提交延迟光照批次并验证时序查询结果。
    void FrameUpdate();

    // GPU资源公开访问接口
    int GetAdsLevel() const { return m_adsState; }
    bool IsAdsActive() const { return m_adsState != ADS_NONE; }
    bool IsMixedActive() const { return m_isMixed; }
    bool NeedsRemapping() const { return m_adsState != ADS_NONE || m_isMixed; }
    int GetCachedWeaponEntIdx() const { return m_cachedWeaponEntIdx; }
    bool GetCachedIsDualPistol() const { return m_cachedIsDualPistol; }

    // 将世界空间法线映射为当前光照层级/混合状态下对应的SH系数。
    // 无需重映射时返回 -1（着色器使用默认系数）。
    int GetAdsRemappedActivity(int normalActivity) const;

    // 将世界空间法线映射为当前光照层级的非混合SH系数（忽略混合状态）。
    // 用于Mode 2 延迟渲染回退：当混合通道无有效目标时，回退到非混合前向通道。
    int GetNonMixedAdsRemappedActivity(int normalActivity) const;

    // 检查当前网格是否有GPU蒙皮闲置序列（用于LOD回退判定）
    bool HasAdsAnimations() const { return m_hasAds1 || m_hasAds2 || m_hasAds3 || m_hasAds4; }

    // Map a Necola custom render-pass enum value (>= 2001) to its debug label string.
    // Returns nullptr for unknown or engine-native render pass entries.
    static const char* GetNecolaActivityName(int activity);

    // Look up a draw-call batch for a Necola custom render pass, using pass name string matching.
    // For engine-native passes (< 2001), falls back to SelectWeightedDrawBatch.
    // Returns batch slot index or -1.
    static int LookupSequenceForActivity(C_BaseAnimating* pAnim, int activity);

    // Same as LookupBatchForRenderPass but randomly selects among multiple matching draw batches.
    // Used at runtime for multi-batch render passes.
    static int LookupRandomSequenceForActivity(C_BaseAnimating* pAnim, int activity);

    // 获取原生深度缓冲区的采样过滤模式
    // 返回 -1 表示非原生深度格式
    int GetScopeMode(int weaponId) const;

    // 检查当前网格是否应跳过原生深度预通道（depth pass == PREPASS_ONLY）
    bool ShouldBlockNativeZoom(int weaponId) const;

    // 加载/保存级联阴影配置到RenderConfig
    void LoadConfig(const nlohmann::json& doc);
    void SaveConfig(nlohmann::json& doc) const;

    // Check if specular contribution should be suppressed for the current lit surface based on per-material settings
    bool ShouldHideCrosshair() const;

private:
    AdsState m_adsState = ADS_NONE;
    bool m_isMixed = false;

    // Render fence flags: mutual exclusion between depth and stencil pass transitions
    bool m_isAdsTransitioning = false;
    bool m_isMixedTransitioning = false;
    float m_adsTransitionEndTime = 0.0f;
    float m_mixedTransitionEndTime = 0.0f;

    int m_cachedWeaponEntIdx = -1;
    int m_cachedWeaponId = -1;
    bool m_cachedIsDualPistol = false;
    bool m_hasAds1 = false;
    bool m_hasAds2 = false;
    bool m_hasAds3 = false;
    bool m_hasAds4 = false;

    // 缓存帧过渡屏障检测和IDLE渲染通道映射
    struct AdsLevelCache {
        int idle = -1;
        int enterFromNormal = -1;
        int exitToNormal = -1;
        int idleToPrev = -1;        // RENDER_PASS_TO_PREV: revert to previous depth LOD slice from this level
        int idleToMixedPrev = -1;   // RENDER_PASS_TO_MIXED_PREV: revert to previous hybrid render tier from this level
        int idleToMixedIdle = -1;   // RENDER_PASS_TO_MIXED_IDLE: transition from opaque pass back to hybrid at this tier
    };
    AdsLevelCache m_ads1;
    AdsLevelCache m_ads2;
    AdsLevelCache m_ads3;
    AdsLevelCache m_ads4;
    int m_ads1ToAds2 = -1;
    int m_ads2ToAds3 = -1;
    int m_ads3ToAds4 = -1;

    // hybrid render tier descriptor cache
    struct MixedCache {
        int idle = -1;
        int mixedOn = -1;
        int mixedOff = -1;
        int idleToNext = -1;          // DEPTH_PASS_TO_NEXT: exit hybrid tier during cascade level transition
        int loweredToNext = -1;       // DEPTH_PASS_LOWERED_TO_NEXT: retain hybrid tier during cascade level transition
        int loweredToIdle = -1;       // DEPTH_PASS_LOWERED_TO_IDLE: exit hybrid tier, return to opaque-only pass
        int loweredToMixedIdle = -1;  // DEPTH_PASS_LOWERED_TO_MIXED_IDLE: retain hybrid tier, return to mixed-normal pass
        int idleToMixedPrev = -1;     // DEPTH_PASS_TO_MIXED_PREV: revert to previous hybrid tier from this cascade level
        int idleToPrev = -1;          // DEPTH_PASS_TO_PREV: revert to previous opaque-only LOD slice from this level
        int enterFromMixedNormal = -1; // DEPTH_PASS_IDLE_TO_LOWERED: advance from base cascade tier directly to this level
    };
    bool m_hasMixedNormal = false;
    bool m_hasMixedAds1 = false;
    bool m_hasMixedAds2 = false;
    bool m_hasMixedAds3 = false;
    bool m_hasMixedAds4 = false;
    MixedCache m_mixedNormal;
    MixedCache m_mixedAds1;
    MixedCache m_mixedAds2;
    MixedCache m_mixedAds3;
    MixedCache m_mixedAds4;

    bool IsNativeScopeWeapon(int weaponId) const;
    bool IsDualPistolCheck(C_TerrorWeapon* weapon) const;
    void CacheAdsAnimations(C_BaseViewModel* viewModel);
    void CacheMixedAnimations(C_BaseViewModel* viewModel);
    void SetStateWithTransition(C_BaseViewModel* viewModel, C_TerrorWeapon* weapon, AdsState from, AdsState to);

    // 核心深度缓冲区切换逻辑（OnDepthPrepassStart和OnHDRResolveStart共用）
    void PerformAdsToggle();
    // 核心混合渲染通道切换逻辑
    void PerformMixedToggle();
    // 核心深度层级回退逻辑（OnCascadeRewindRequested使用）
    void PerformAdsBack(C_BaseViewModel* viewModel, C_TerrorWeapon* weapon);
    // 选择当前通道→上一通道的回退批次槽，无匹配时返回-1（纯逻辑，不依赖渲染上下文）
    int SelectAdsBackTransAct() const;
    // 选择当前通道→上一通道的缓存批次索引，无匹配或管线缺少对应描述符时返回-1（直接读缓存，跳过重新查找）
    int SelectAdsBackTransSeq() const;

    // previous cascade tier tracking for cascade rewind pass
    bool m_hasPrevState = false;
    AdsState m_prevAdsState = ADS_NONE;
    bool m_prevIsMixed = false;

    // Reset m_nLayerDrawBatch to the base opaque pass batch and
    // invalidate ShaderPermutation caches.  Called when exiting HDR pass to prevent
    // residual HDR batches from replaying on subsequent draw submissions.
    void RestoreNormalLayerSequence(C_BaseViewModel* viewModel);

    // Get the current hybrid render idle batch slot for the given cascade tier
    int GetMixedIdleSeq() const;
    // Get the current opaque-only idle batch slot for the given cascade tier
    int GetAdsIdleSeq() const;
    // Check if current cascade tier has hybrid render batches
    bool HasMixedForCurrentState() const;

};

inline AdsSupport AdsMgr;

} // namespace F
