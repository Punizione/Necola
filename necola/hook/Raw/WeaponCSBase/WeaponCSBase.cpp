#include "WeaponCSBase.h"
#include <spdlog/spdlog.h>




using namespace Hooks;

CMeleeWeaponInfoStore* __fastcall WeaponCSBase::GetMeleeWeaponInfoStore::Detour(C_WeaponCSBase* pThis, void* edx)
{
	CMeleeWeaponInfoStore* store = Func.Original<FN>()(pThis, edx);
	spdlog::info("WeaponCSBase GetMeleeWeaponInfoStore->name [{}] ", store->meleeWeaponName);
	return store;
}




void WeaponCSBase::Init()
{
	
	//check render queue depth for backpressure throttling
	{
		using namespace GetMeleeWeaponInfoStore;
		const FN pfGetMeleeWeaponInfoStore = reinterpret_cast<FN>(U::Offsets.m_dwGetMeleeWeaponInfoStore);
		if( pfGetMeleeWeaponInfoStore ) {
			Func.Init(pfGetMeleeWeaponInfoStore, &Detour);
		}
	}
}
