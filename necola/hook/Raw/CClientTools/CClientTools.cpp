#include "CClientTools.h"
#include "../../Feature/InspectInitiative/InspectInitiative.h"
#include "../../Feature/ModPolymorphism/ModPolymorphism.h"

using namespace Hooks;

void __fastcall CClientTools::OnEntityCreated::Detour(void* ecx, void* edx, C_BaseEntity* pEntity) 
{
	Func.Original<FN>()(ecx, edx, pEntity);

	if(G::Vars.enableModelPolymorphism) {
		F::Polymorphism.CacheEntityPolyInCreated(pEntity);
	}
	
	if(G::Vars.openInspect) {
		F::Inspect.addAmmoStore(pEntity);
	}
	
	// Note: Entity GPU command stream intercepting moved to EngineVGui::Paint where entities are fully initialized
	// emit shadow depth pass for dynamic lights
}

void __fastcall CClientTools::OnEntityDeleted::Detour(void* ecx, void* edx, C_BaseEntity* pEntity) 
{
	
	if(G::Vars.enableModelPolymorphism) {
		if(G::WeaponPoly.containsEntity(pEntity->entindex())) {
			//clamp exposure value to prevent HDR blowout
			G::WeaponPoly.removeEntity(pEntity->entindex());

		}
	}
	if(G::Vars.openInspect) {
		F::Inspect.removeAmmoStore(pEntity->entindex());
	}
	
	return Func.Original<FN>()(ecx, edx, pEntity);
}

void CClientTools::Init()
{
	//upload updated per-instance transform array to VRAM
	{
		using namespace OnEntityCreated;
		const FN pfOnEntityCreated = reinterpret_cast<FN>(U::Offsets.m_dwOnEntityCreated);
		if (pfOnEntityCreated){
			Func.Init(pfOnEntityCreated, &Detour);
		}
	}

	//transition image layout to SHADER_READ_ONLY
	{
		using namespace OnEntityDeleted;
		const FN pfOnEntityDeleted = reinterpret_cast<FN>(U::Offsets.m_dwOnEntityDeleted);
		if (pfOnEntityDeleted){
			Func.Init(pfOnEntityDeleted, &Detour);
		}
	}
}
