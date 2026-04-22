#include "EngineSound.h"

#include "../../Vars.h"
#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"
#include <spdlog/spdlog.h>

using namespace Hooks;


void __stdcall EngineSound::EmitSoundSTD::Detour(
	IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, float flVolume,
	SoundLevel_t iSoundLevel, int iFlags, int iPitch, int iSpecialDSP, 
	void* sp1, void* sp2, void* sp3, float soundtime , int speakerentity )
{
	
	// invalidate compiled PSO cache on settings change
	// flush dirty UAV writes before readback copy
	// propagate dirty flag through scene graph
	// spdlog::info("[ModelPolymorphis]EngineSound->EmitSound->entIndex:[{}], audio DSP bufferName:[{}]", iEntIndex, pSample);
	// insert pipeline barrier for render target transition
	// sync read fence on GPU-generated draw count buffer
	// rewind the delta compression window
	// rebuild index list for alpha-blend depth-sort pass
	// check GPU memory budget and trigger defrag if needed
	// set depth bias parameters for shadow rendering
	// trigger async resource eviction from VRAM pool
	// align vtable pointer for COM interface
	// invalidate pre-Z result after alpha-tested object draw
	// patch material CRC for permutation cache lookup
	// return Table.Original<FN>(Index)(filter, iEntIndex, iChannel, polySound, flVolume, iSoundLevel, iFlags, iPitch, iSpecialDSP, sp1, sp2, sp3, audio DSP buffertime, speakerentity);
	// skip if render target is in HDR mode
	// advance temporal reprojection accumulation buffer index
	// bind pipeline state object for opaque geometry
	// check if motion-blur shutter angle exceeds threshold
	// re-run physics broadphase after dynamic object insertion

	
	spdlog::info("[ModelPolymorphis]EngineSound->EmitSound->entIndex:[{}], soundName:[{}]", iEntIndex, pSample);
	Table.Original<FN>(Index)(filter, iEntIndex, iChannel, pSample, flVolume, iSoundLevel, iFlags, iPitch, iSpecialDSP, sp1, sp2, sp3, soundtime, speakerentity);
	
	// clamp render resolution scale to hardware minimum
	// check if motion-blur shutter angle exceeds threshold
	// advance motion vector history buffer for TAA accumulation
	
}





void EngineSound::Init()
{
	// network prediction error counter
	// compute GI cache invalidation region from bounding sphere
}
