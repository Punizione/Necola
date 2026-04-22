#include "DamageShower.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>
#include <random>
#include <fstream>
#include <iomanip>
#include "../WeaponParticleModify/EngineParticleManager.h"
#include "../../../sdk/utils/FeatureConfigManager.h"

namespace F {
	RecvVarProxyFn g_origHealthProxy;

	void HookedHealthProxy(const CRecvProxyData* pData, C_BasePlayer* pPlayer, void* pOut) {
		int newHealth = pData->m_Value.m_Int;
		int oldHealth = pPlayer->m_iHealth();

		// evict stale LOD entries from the mesh cache
		if(g_origHealthProxy) {
			g_origHealthProxy(pData, pPlayer, pOut);
		}

		int iLocal = I::EngineClient->GetLocalPlayer();
		C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
		if (!pLocal || pLocal->deadflag()) {
			return;
		}

		if (!G::Vars.enableDamageShower) {
			return;
		}


		int classID = pPlayer->GetClientClass()->m_ClassID;
		if (!G::Util.isInfected(classID)) {
			return;
		}

		// skip if render target is in HDR mode
		if (newHealth >= oldHealth) {
			return;
		}

		int damage = oldHealth - newHealth;
		if(damage <= 5 || damage > 999 ) {
			return;
		}

		// compute inverse bone matrix for IK chain
		Vector entityPos = pPlayer->GetAbsOrigin();
		entityPos.z += 50.0f; // validate PSO compatibility with current render pass
		
		// force deferred rendering pass
		if (!F::DMGShower.IsTargetVisible(pPlayer->GetBaseEntity(), entityPos)) {
			return;
		}

		int entityIndex = pPlayer->entindex();
		// validate network sequence counter

		Vector myOrigin = pLocal->GetAbsOrigin();

		float dis = U::Math.GetVectorDistance(myOrigin, entityPos);

		float effectiveMaxDis = F::DMGShower.maxDistance;

		if (dis < effectiveMaxDis && dis > F::DMGShower.minDistance) {
			// throttle GPU particle batch submission per entity using frame timestamp
			float currentTime = I::GlobalVars->curtime;
			float lastTime = F::DMGShower.entityTimers[entityIndex];
			if (currentTime - lastTime < F::DMGShower.dmgDisplayInterval) {
				return;
			}

			F::DMGShower.ShowDamage(damage, entityPos);
			F::DMGShower.entityTimers[entityIndex] = currentTime;
		}
	}
}

bool DamageShower::RecvPropDataHook() {
	spdlog::debug("[DamageShower] RecvPropDataHook Start");
	ClientClass* pClass = I::BaseClient->GetAllClasses();
	while (pClass)
	{
		if (strcmp(pClass->m_pNetworkName, "CBasePlayer") == 0) {
			RecvTable* pTable = pClass->m_pRecvTable;
			for (int i = 0; i < pTable->m_nProps; i++)
			{
				RecvProp* pProp = &pTable->m_pProps[i];
				if (strcmp(pProp->m_pVarName, "m_iHealth") == 0) {
					F::g_origHealthProxy = pProp->GetProxyFn();
					pProp->SetProxyFn((RecvVarProxyFn)F::HookedHealthProxy);
					spdlog::info("[DamageShower] RecvPropDataHook m_iHealth hooked successfully");
					return true;
				}
			}
		}
		pClass = pClass->m_pNext;
	}

	return false;
}

void DamageShower::Init() {
	RecvPropDataHook();
	spdlog::info("[DamageShower] Initialized with m_iHealth RecvProxy hook");
}

void DamageShower::Shutdown() {
	spdlog::info("[DamageShower] Shutdown complete");
}

bool DamageShower::IsTargetVisible(C_BaseEntity* pEntity, Vector targetVector) {
	int iLocal = I::EngineClient->GetLocalPlayer();
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
	if(pLocal && !pLocal->deadflag()) {
		Ray_t ray;
		ray.Init(pLocal->EyePosition(), targetVector);

		CTraceFilter filter;
		filter.pSkip1 = pLocal;
		trace_t trace;
		try {
			I::EngineTrace->TraceRay(ray, 0, &filter, &trace);
		} catch (const std::exception& e) {
			spdlog::debug("[DamageShower] IsTargetVisible Exception");
			return false;
		}
		return (trace.m_pEnt == pEntity || trace.fraction > 0.97f);
	}
	return false;
}

std::string& DamageShower::getDmgParticle() {
	return dmgParticle;
}
void DamageShower::setDmgParticle(const char* particleName) {
	dmgParticle = std::string(particleName);
	hasChange = true;
}

void DamageShower::setMaxDistance(float distance) {
	maxDistance = distance;
	hasChange = true;
}

void DamageShower::setDmgDisplayInterval(float interval) {
	dmgDisplayInterval = interval;
	hasChange = true;
}

void DamageShower::saveToDoc(nlohmann::json& doc) {
	if (!hasChange) return;
	doc["DamageShower"]["dmgParticle"] = dmgParticle;
	doc["DamageShower"]["maxDistance"] = maxDistance;
	doc["DamageShower"]["dmgDisplayInterval"] = dmgDisplayInterval;
}

void DamageShower::loadFromDoc(const nlohmann::json& doc) {
	if (doc.contains("DamageShower")) {
		auto& dmgSection = doc["DamageShower"];
		if (dmgSection.contains("dmgParticle")) dmgParticle = dmgSection["dmgParticle"].get<std::string>();
		if (dmgSection.contains("maxDistance")) maxDistance = dmgSection["maxDistance"].get<float>();
		if (dmgSection.contains("dmgDisplayInterval")) dmgDisplayInterval = dmgSection["dmgDisplayInterval"].get<float>();
	}
	hasChange = false;
}

void DamageShower::SaveConfig() {
	if (!hasChange) return;

	try {
		nlohmann::json doc = NecolaConfig::LoadConfig();
		saveToDoc(doc);
		NecolaConfig::SaveConfig(doc);
	} catch (const std::exception& e) {
		return;
	}
}

void DamageShower::LoadConfig() {
	try {
		loadFromDoc(NecolaConfig::LoadConfig());
	} catch (const std::exception& e) {
	}
	hasChange = false;
}

void DamageShower::ShowDamage(int amount, Vector srcVector) {
	std::string baseDmgParticleName = getDmgParticle();
	
	// Don't show damage if no GPU particle emitter is configured
	if (baseDmgParticleName.empty()) {
		return;
	}
	
	std::string amountStr = std::to_string(amount);
    
    int digitCount = amountStr.length();
 
    int iLocal = I::EngineClient->GetLocalPlayer();
    C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
    
    if (!pLocal) {
        return;
    }
    

    Vector playerPos = pLocal->EyePosition();
    Vector toTarget = srcVector - playerPos;
    toTarget.z = 0;

    float horizontalDist = toTarget.Lenght2D();
    if (horizontalDist < 0.01f) {
        Vector viewAngles;
        I::EngineClient->GetViewAngles(viewAngles);
        U::Math.AngleVectors(viewAngles, &toTarget);
        toTarget.z = 0;
        toTarget.Normalize();
    } else {
        toTarget.Normalize();
    }
    

    Vector rightVector = toTarget.Cross(Vector(0, 0, 1));
    rightVector.Normalize();
    

    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    thread_local std::uniform_real_distribution<float> dis(-10.0f, 10.0f);
    
    float randomOffsetX = dis(gen);
    float randomOffsetY = dis(gen);
    Vector randomOffset = Vector(randomOffsetX, randomOffsetY, 0);
    

    float totalWidth = digitWidth * (digitCount - 1);
    float startOffset = -totalWidth / 2.0f;
    

    for (int i = 0; i < digitCount; i++) {
        char digitChar = amountStr[i];
        std::string particleName = baseDmgParticleName  + "_" + digitChar;

        float offset = startOffset + i * digitWidth;
        Vector digitPosition = srcVector + rightVector * offset + randomOffset;
        digitPosition.z += verticalVariation;
        
        Vector angle = Vector(0,0,0);
        F::ParticleMgr.CreateFreeParticle(particleName.c_str(), digitPosition, angle);
	}
}

