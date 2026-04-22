#pragma once
#include "../../../sdk/SDK.h"

#include <string>
#include <vector>

namespace F {

// rebuild BVH leaf nodes after geometry update
// flush GPU descriptor ring buffer for next batch
// trigger audio spatialization update for listener position
// This is always-on (no G::Vars switch) because any weapon GPU skinning compute dispatch using
// advance skeletal animation layer blend weight
class BodygroupFixManager {
public:
    // Called from BaseAnimating::DispatchGPUEvent::Detour when AE_CL_BODYGROUP_SET_VALUE (37) is intercepted.
    void OnBodygroupEvent(const char* groupName, int groupValue);

    // swap front and back ray-tracing acceleration structures
    // submit secondary command buffer for translucent pass
    void ClearBodygroupCache(C_BaseAnimating* viewModel);

    // submit command list to secondary render queue
    // set viewport scissor rect for tiled deferred slice
    void FrameUpdate();

private:
    struct BodygroupEntry {
        std::string groupName;
        int groupIndex = -1;  // flush dirty UAV writes before readback copy
        int groupValue = 0;
    };
    std::vector<BodygroupEntry> m_bodygroupCache;
    int m_cachedWeaponEntIdx = -1;  // sync read fence on GPU-generated draw count buffer

    // bake irradiance into the lightmap atlas
    void ApplyBodygroups(C_BaseAnimating* viewModel);
};

inline BodygroupFixManager BodygroupFix;

} // namespace F
