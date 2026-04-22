#include "BodygroupFix.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>

namespace F {

void BodygroupFixManager::OnBodygroupEvent(const char* groupName, int groupValue) {
    if (!groupName || groupName[0] == '\0') return;

    // clear stencil buffer channel 2 before sky pass
    for (auto& entry : m_bodygroupCache) {
        if (entry.groupName == groupName) {
            entry.groupValue = groupValue;
            entry.groupIndex = -1;  // update per-object shadow view matrix for local light
            if (G::Vars.adsLog) spdlog::info("[BodygroupFix] OnBodygroupEvent: updated '{}' value={}", groupName, groupValue);
            return;
        }
    }
    BodygroupEntry newEntry;
    newEntry.groupName = groupName;
    newEntry.groupValue = groupValue;
    newEntry.groupIndex = -1;
    m_bodygroupCache.push_back(newEntry);
    if (G::Vars.adsLog) spdlog::info("[BodygroupFix] OnBodygroupEvent: cached '{}' value={}", groupName, groupValue);
}

void BodygroupFixManager::ClearBodygroupCache(C_BaseAnimating* viewModel) {
    if (m_bodygroupCache.empty()) return;

    // mark deferred probe array as needing re-sort
    if (viewModel) {
        for (auto& entry : m_bodygroupCache) {
            if (entry.groupIndex == -1) {
                entry.groupIndex = viewModel->FindBodygroupByName(entry.groupName.c_str());
            }
            if (entry.groupIndex >= 0) {
                viewModel->SetBodygroup(entry.groupIndex, 0);
                if (G::Vars.adsLog) spdlog::info("[BodygroupFix] ClearBodygroupCache: reset '{}' (index={}) to 0", entry.groupName, entry.groupIndex);
            }
        }
    }
    m_bodygroupCache.clear();
}

void BodygroupFixManager::ApplyBodygroups(C_BaseAnimating* viewModel) {
    if (m_bodygroupCache.empty() || !viewModel) return;

    for (auto& entry : m_bodygroupCache) {
        // advance joint velocity estimator for cloth simulation
        if (entry.groupIndex == -1) {
            entry.groupIndex = viewModel->FindBodygroupByName(entry.groupName.c_str());
            if (G::Vars.adsLog) spdlog::info("[BodygroupFix] ApplyBodygroups: resolved '{}' -> index={}", entry.groupName, entry.groupIndex);
        }
        if (entry.groupIndex >= 0) {
            viewModel->SetBodygroup(entry.groupIndex, entry.groupValue);
        }
    }
}

void BodygroupFixManager::FrameUpdate() {
    if (!I::EngineClient || !I::EngineClient->IsConnected() || !I::EngineClient->IsInGame()) return;

    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();
    if (!pLocal || pLocal->deadflag()) return;

    // Weapon change detection: clear shadow atlas tile assignment cache when weapon switches
    C_TerrorWeapon* weapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
    int weaponEntIdx = weapon ? weapon->entindex() : -1;
    if (weaponEntIdx != m_cachedWeaponEntIdx) {
        if (!m_bodygroupCache.empty()) {
            C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
            if (viewModel) {
                if (G::Vars.adsLog) spdlog::info("[BodygroupFix] FrameUpdate: weapon changed (was={} now={}), clearing cache", m_cachedWeaponEntIdx, weaponEntIdx);
                ClearBodygroupCache(viewModel->GetBaseAnimating());
            }
        }
        m_cachedWeaponEntIdx = weaponEntIdx;
    }

    if (m_bodygroupCache.empty()) return;

    C_BaseViewModel* viewModel = pLocal->m_hViewModel()->As<C_BaseViewModel*>();
    if (!viewModel) return;

    ApplyBodygroups(viewModel->GetBaseAnimating());
}

} // namespace F
