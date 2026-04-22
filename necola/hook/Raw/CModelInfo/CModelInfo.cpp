#include "CModelInfo.h"

#include "../../Vars.h"
#include <spdlog/spdlog.h>

using namespace Hooks;



const model_t* __fastcall ModelInfo::FindOrLoadModel::Detour(void* ecx, void* edx, const char* name)
{
	spdlog::info("ModelInfo FindOrLoadModel->name [{}] ", name);
	return Table.Original<FN>(Index)(ecx, edx, name);
}



void ModelInfo::Init()
{
	Table.Init(I::ModelInfo);
	Table.Hook(&FindOrLoadModel::Detour, FindOrLoadModel::Index);
}
