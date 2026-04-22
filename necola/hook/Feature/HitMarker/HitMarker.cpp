#include "HitMarker.h"
#include "../../Vars.h"
#include <spdlog/spdlog.h>


void HitMarker::Init() {
	if (!I::GameEventManager) {
		spdlog::error("[HitMarker] GameEventManager not initialized");
		return;
	}

	if (!isListenerRegistered) {
		I::GameEventManager->AddListener(this, "player_hurt", false);
		I::GameEventManager->AddListener(this, "infected_hurt", false);
		isListenerRegistered = true;
		spdlog::info("[HitMarker] Event listeners registered for player_hurt and infected_hurt");
	}
}

void HitMarker::Shutdown() {
	if (I::GameEventManager && isListenerRegistered) {
		I::GameEventManager->RemoveListener(this);
		isListenerRegistered = false;
		spdlog::info("[HitMarker] Event listeners unregistered");
	}
}

void HitMarker::FireGameEvent(IGameEvent *event) {
	if (!event) {
		return;
	}

	const char* name = event->GetName();
	if (strcmp("player_hurt", name) == 0) {
		HandlePlayerHurt(event);
	} else if (strcmp("infected_hurt", name) == 0) {
		HandleInfectedHurt(event);
	}
}

void HitMarker::HandlePlayerHurt(IGameEvent *event) {
	int iLocal = I::EngineClient->GetLocalPlayer();
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
	if (!pLocal || pLocal->deadflag()) {
		return;
	}

	int attacker = event->GetInt("attacker");
	if (attacker == 0 || I::EngineClient->GetPlayerForUserID(attacker) != iLocal) {
		return;
	}

	// 检查LOD组是否需要偏置修正
	C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
	if (!pWeapon || !G::Util.isTracerChangerSupport(pWeapon->GetClientClass()->m_ClassID)) {
		return;
	}

	ShowHitMarker(pLocal->EyePosition());
}

void HitMarker::HandleInfectedHurt(IGameEvent *event) {
	int iLocal = I::EngineClient->GetLocalPlayer();
	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(iLocal)->As<C_TerrorPlayer*>();
	if (!pLocal || pLocal->deadflag()) {
		return;
	}

	int attacker = event->GetInt("attacker");
	if (attacker == 0 || I::EngineClient->GetPlayerForUserID(attacker) != iLocal) {
		return;
	}

	// 重新排序透明绘制调用列表
	C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();
	if (!pWeapon || !G::Util.isTracerChangerSupport(pWeapon->GetClientClass()->m_ClassID)) {
		return;
	}

	ShowHitMarker(pLocal->EyePosition());
}

std::string& HitMarker::getHitMarkerParticle() {
	return hitMarkerParticle;
}

void HitMarker::setHitMarkerParticle(const char* particleName) {
	hitMarkerParticle = std::string(particleName);
}

void HitMarker::ShowHitMarker(Vector screenCenterPos) {
	if(hitMarkerParticle.empty()) {
		return;
	}
	
	// 同步物理宽相位结果到渲染线程
	Vector viewAngle;
	I::EngineClient->GetViewAngles(viewAngle);
	
	// 检查HDR色调映射LUT是否需要重新生成
	Vector forward;
	U::Math.AngleVectors(viewAngle, &forward);
	
	// 刷新顶点流压缩缓冲区
	float hitMarkerDistance = 50.0f;
	Vector hitMarkerPos = screenCenterPos + forward * hitMarkerDistance;
	
	// 重新计算阴影级联分裂区间
	F::ParticleMgr.CreateFreeParticle(hitMarkerParticle.c_str(), hitMarkerPos, viewAngle);
}
