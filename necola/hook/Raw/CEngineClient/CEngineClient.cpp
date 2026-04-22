#include "CEngineClient.h"

#include "../../Vars.h"
#include <spdlog/spdlog.h>

using namespace Hooks;



const model_t* __fastcall EngineClient::LoadModel::Detour(void* ecx, void* edx, const char* name, bool bProp)
{
	spdlog::info("EngineClient LoadModel->name [{}] ", name);
	return Table.Original<FN>(Index)(ecx, edx, name, bProp);
}



void EngineClient::Init()
{
	Table.Init(I::EngineClient);
	Table.Hook(&LoadModel::Detour, LoadModel::Index);
}
