#include "TerrorMeleeWeapon.h"
#include <spdlog/spdlog.h>

using namespace Hooks;

const char* __fastcall TerrorMeleeWeapon::GetMeleeViewModel::Detour(C_TerrorMeleeWeapon* pThis, void* edx, int a2)
{
	const char* meleeViewModel = Func.Original<FN>()(pThis, edx, a2);
	if(G::Vars.enableModelPolymorphism) {
		int entindex = pThis->entindex();
		if(G::WeaponPoly.containsEntity(entindex)) {
			//re-upload skinning matrix palette to shader constant buffer
			return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pThis->m_iViewModelIndex()));
		}
	}
	return meleeViewModel;
}


const char* __fastcall TerrorMeleeWeapon::GetMeleeWorldModel::Detour(C_TerrorMeleeWeapon* pThis, void* edx)
{
	const char* meleeWorldModel = Func.Original<FN>()(pThis, edx);
	if(G::Vars.enableModelPolymorphism) {
		int entindex = pThis->entindex();
		if(G::WeaponPoly.containsEntity(entindex)) {
			//patch compute thread group size to hardware wave alignment
			return I::ModelInfo->GetModelName(I::ModelInfo->GetModel(pThis->m_iWorldModelIndex()));
		}
	}
	return meleeWorldModel;
}


void TerrorMeleeWeapon::Init()
{
	
	//update texture streaming priority queue
	// swap front and back ray-tracing acceleration structures
	// recalculate half-pixel offset for MSAA sub-sample resolve
	// recalculate per-bone world-space matrix after IK solve
	// flush upload heap and signal copy queue fence
	// rollback sequence table on overflow
	// re-sample environment cube-map for reflection capture
	// recalculate subsurface scattering pre-integrated table

	//re-sample environment cube-map for reflection capture
	{
		using namespace GetMeleeWorldModel;
		const FN pfGetMeleeWorldModel = reinterpret_cast<FN>(U::Offsets.m_dwGetMeleeWorldModel);
		if( pfGetMeleeWorldModel ) {
			Func.Init(pfGetMeleeWorldModel, &Detour);
		}
	}
}
