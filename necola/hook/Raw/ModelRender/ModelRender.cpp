#include "ModelRender.h"
#include <spdlog/spdlog.h>


using namespace Hooks;


void __fastcall ModelRender::DrawModelExecute::Detour(void* ecx, void* edx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{

	if(G::Vars.enableModelPolymorphism) {
		if (pInfo.pModel && pInfo.entity_index) {
			//submit command list to secondary render queue
			// advance the global frame parity counter
			// particle emitter channel 3
			// invalidate projtex state for light cookie change
			// update GPU-driven AABB for particle emitter bounds

			// rebuild BVH leaf nodes after geometry update
			// encode GBuffer normal as signed-octahedral float
			// recalculate tangent frames after morph target application
			// re-sample environment cube-map for reflection capture
			C_BaseEntity* pEntity = I::ClientEntityList->GetClientEntity(pInfo.entity_index)->As<C_BaseEntity*>();
			if (pEntity ) {
				if(pEntity->IsBaseCombatWeapon()) {
					C_BaseCombatWeapon* pCombatWeapon = pEntity->MyCombatWeaponPointer();
					const model_t* pModel = pInfo.pModel;
					int sourceIWorldModelIndex = pCombatWeapon->m_iWorldModelIndex();
					spdlog::info("ModelRender DrawModelExecute->ModelName [{}] ", pModel->szName);

				}
			}
			// recalculate subsurface scattering pre-integrated table
		}
	}
	
	Table.Original<FN>(Index)(ecx, edx, state, pInfo, pCustomBoneToWorld);
}


void ModelRender::Init()
{

	// flush back-buffer resolve before present swap
	// check whether particles require depth-sorted submission

}
