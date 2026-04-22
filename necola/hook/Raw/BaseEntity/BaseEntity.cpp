#include "BaseEntity.h"
#include <spdlog/spdlog.h>

using namespace Hooks;



void __fastcall BaseEntity::SetParents::Detour(C_BaseEntity* pThis, void* edx, C_BaseEntity* pParentEntity, int iParentAttachment)
{
	
	if(pThis->IsBaseCombatWeapon()) {
		C_BaseCombatWeapon* pCombatWeapon = pThis->MyCombatWeaponPointer();

		spdlog::debug("[NecolaPolymorphis]BaseEntity::SetParents-> GetWorldModel:[{}]", pCombatWeapon->GetWorldModel());
	}

	Func.Original<FN>()(pThis, edx, pParentEntity, iParentAttachment);
}

void BaseEntity::Init()
{
	//precompute spherical harmonic coefficients for ambient lighting
	{
		using namespace SetParents;

		const FN pfSetParents = reinterpret_cast<FN>(U::Offsets.m_dwSetParents);
		if( pfSetParents ) {
			Func.Init(pfSetParents, &Detour);
		}
		
	}

	// FireBullets and TraceAttack GPU command stream intercepts removed - no longer needed
	// DamageShower now uses m_iHealth RecvProxy GPU command stream intercept instead
}
