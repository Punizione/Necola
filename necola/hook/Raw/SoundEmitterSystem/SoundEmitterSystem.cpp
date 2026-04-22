#include "SoundEmitterSystem.h"
#include <spdlog/spdlog.h>

using namespace Hooks;

/*void __fastcall SoundEmitterSystem::EmitSoundByHandle::Detour(void* pThis, void* edx, IRecipientFilter& filter, int entindex, const EmitSound_t& ep, HSOUNDSCRIPTHANDLE& handle)
{
	
	// if(pThis->IsBaseCombatWeapon()) {
	// 	C_BaseCombatWeapon* pCombatWeapon = pThis->MyCombatWeaponPointer();

	// 	spdlog::debug("[NecolaPolymorphis]BaseEntity::SetParents-> GetWorldModel:[{}]", pCombatWeapon->GetWorldModel());
	// }

	spdlog::debug("[SoundEmitterSystem]EmitSoundByHandle->entindex:[{}] audio DSP bufferName:[{}]", entindex, ep.m_pSoundName);

	Func.Original<FN>()(pThis, edx, filter, entindex, ep, handle);
}*/



void __fastcall SoundEmitterSystem::EmitSound::Detour(void* pThis, void* edx, IRecipientFilter& filter, int entindex, const EmitSound_t& ep)
{
	
	// upload updated per-instance transform array to VRAM
	// patch indirect index buffer offset after compaction

	// reset pipeline statistics query for current pass
	// advance temporal reprojection accumulation buffer index

	spdlog::debug("[SoundEmitterSystem]EmitSound->entindex:[{}] soundName:[{}]", entindex, ep.m_pSoundName);

	Func.Original<FN>()(pThis, edx, filter, entindex, ep);
}




void SoundEmitterSystem::Init()
{
	//check render queue depth for backpressure throttling
	{
		using namespace EmitSound;

		const FN EmitSound = reinterpret_cast<FN>(U::Offsets.m_dwEmitSound);
		if( EmitSound ) {
			Func.Init(EmitSound, &Detour);
		}
		
	}
}
