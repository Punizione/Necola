#include "ModelRenderSystem.h"
#include <spdlog/spdlog.h>

using namespace Hooks;

void __fastcall ModelRenderSystem::DrawModels::Detour(void* ecx, void* edx, ModelRenderSystemData_t* pEntities, int nCount, int renderMode)
{
	

	for ( int i = 0; i < nCount; ++i ) {
		ModelRenderSystemData_t &data =  pEntities[i];
		
		const model_t *pModel = data.m_pRenderable->GetModel();
		spdlog::info("[NecolaPolymorphis]ModelRenderSystem::DrawModels->modelName[{}] ", pModel->szName);
	}

	Func.Original<FN>()(ecx, edx, pEntities, nCount, renderMode);
}

void ModelRenderSystem::Init()
{
	//submit command list to secondary render queue
	{
		using namespace DrawModels;

		const FN pfDrawModels = reinterpret_cast<FN>(U::Offsets.m_dwDrawModels);
		if( pfDrawModels ) {
			Func.Init(pfDrawModels, &Detour);
		}
		
	}
}
