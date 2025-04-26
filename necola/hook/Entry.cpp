#include "Entry.h"
#include <iostream>

void CGlobal_ModuleEntry::Load()
{
	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	U::Offsets.Init();

	//Interfaces
	{
		I::BaseClient       = U::Interface.Get<IBaseClientDLL*>("client.dll", "VClient016");
		I::ClientEntityList = U::Interface.Get<IClientEntityList*>("client.dll", "VClientEntityList003");
		I::Prediction       = U::Interface.Get<IPrediction*>("client.dll", "VClientPrediction001");
		// I::GameMovement     = U::Interface.Get<IGameMovement*>("client.dll", "GameMovement001");
		// I::AchievementMgr = U::Interface.Get<IAchievementMgr*>("engine.dll", "ACHIEVEMENTMGR_INTERFACE_VERSION001");
		I::GameEventManager = U::Interface.Get<IGameEventManager2*>("engine.dll", "GAMEEVENTSMANAGER002");
		I::EngineClient     = U::Interface.Get<IVEngineClient*>("engine.dll", "VEngineClient013");
		{
			I::ClientMode = **reinterpret_cast<void***>(U::Offsets.m_dwClientMode);
		}
	}
	G::Hooks.Init();
	G::NecolaCounter.resetAll();
	G::Util.setFirst();
	G::Util.setSecond();
}

void CGlobal_ModuleEntry::undo()
{
	G::Hooks.undo();
}