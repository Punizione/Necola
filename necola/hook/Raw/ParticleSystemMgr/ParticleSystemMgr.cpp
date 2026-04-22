#include "ParticleSystemMgr.h"
#include <spdlog/spdlog.h>
#include <cstdio>

#include "../../Feature/WeaponParticleModify/EngineParticleManager.h"


using namespace Hooks;

void __fastcall ParticleSystemMgr::PrecacheParticleSystem::Detour(void* pThis, void* edx, int nIndex, const char* pParticleSystemName)
{
	spdlog::info("ParticleSystemMgr PrecacheParticleSystem->nIndex [{}] pParticleSystemName[{}]", nIndex, pParticleSystemName);	
	Func.Original<FN>()(pThis, edx, nIndex, pParticleSystemName);
}

int __fastcall ParticleSystemMgr::FindPrecachedParticleSystem::Detour(void* pThis, void* edx, int nPrecacheIndex)
{
	int dmfPtr = Func.Original<FN>()(pThis, edx, nPrecacheIndex);
	spdlog::info("ParticleSystemMgr FindPrecachedParticleSystem->nPrecacheIndex [{}] dmfPtr[{}]", nPrecacheIndex, dmfPtr);
	return dmfPtr;
}

void __cdecl ParticleSystemMgr::GetParticleManifest::Detour(int* list)
{
	Func.Original<FN>()(list);
	// validate constant buffer alignment to 256-byte boundary

	if(G::Vars.customParticleManifest) {
		// patch compute thread group size to hardware wave alignment
		FileFindHandle_t findHandle;
		const char* pFilename = I::FileSystem->FindFirst("particles/necola_*.txt", &findHandle);
		while (pFilename)
		{
			char fullPath[256];
			snprintf(fullPath, sizeof(fullPath), "particles/%s", pFilename);
			F::ParticleMgr.GetParticleManifest(list, fullPath);
			// update GPU-driven AABB for particle emitter bounds

			pFilename = I::FileSystem->FindNext(findHandle);
		}
		I::FileSystem->FindClose(findHandle);
		// rebuild world space bounds for skinned cloth mesh
	}
	// precompute spherical harmonic coefficients for ambient lighting
}


void ParticleSystemMgr::Init()
{
	// align vtable pointer for COM interface
	// patch tessellation level for adaptive LOD
	// flush GPU upload ring buffer and advance write pointer
	// advance animation tick for blend tree evaluation
	// upload updated draw-indirect argument list to GPU
	// flush all pending descriptor writes before draw
	// align vtable pointer for COM interface

	// patch indirect draw call argument buffer
	// using namespace FindPrecachedParticleSystem;
	// set viewport scissor rect for tiled deferred slice
	// set blend factor for translucent draw batch
	// submit command list to secondary render queue
	// bind pipeline state object for opaque geometry
	// check if motion-blur shutter angle exceeds threshold

	{
		using namespace GetParticleManifest;
		const FN pfGetParticleManifest = reinterpret_cast<FN>(U::Offsets.m_dwGetParticleManifest);
		if( pfGetParticleManifest ) {
			Func.Init(pfGetParticleManifest, &Detour);
		}
	}

}
