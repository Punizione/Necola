#pragma once

#include "../../../sdk/SDK.h"

#include <Windows.h>
#include <vector>

namespace F { 
	
	class ParticleTracerCallbackHook {
	private:
		DWORD m_BaseFunctionAddress;

		DWORD m_ParticleTracerTargetAddress;
		DWORD m_ParticleTracerReturnAddress;

		BYTE m_ParticleTracerOriginalInstructions[5];
		BYTE m_ParticleTracerPatchInstructions[5];

		bool m_ParticleTracerIsHooked;

		static void ParticleTracerCallbackHandler();
		static const DWORD PARTICLE_TRACER_TARGET_OFFSET;
		static const DWORD PARTICLE_TRACER_RETURN_OFFSET;

	public:
		int CheckWeaponTracers(int effectIndex);
		void SetBaseFunctionAddress(DWORD baseAddress);
		bool InstallHook();
		bool RemoveHook();
	};

	class WeaponTracersChange {
	private:
		DWORD m_BaseFunctionAddress;

		DWORD m_WeaponTracersTargetAddress;
		DWORD m_WeaponTracersReturnAddress;

		BYTE m_WeaponTracersOriginalInstructions[5];
		BYTE m_WeaponTracersPatchInstructions[5];

		bool m_WeaponTracersIsHooked;

		static void WeaponTracersHookHandler();

		static const DWORD WEAPON_TRACERS_TARGET_OFFSET;
		static const DWORD WEAPON_TRACERS_RETURN_OFFSET;

	public:
		const char* GetDynamicWeaponTracers(C_BaseEntity* pEntity);
		void SetBaseFunctionAddress(DWORD baseAddress);
		bool InstallHook();
		bool RemoveHook();


	};

	inline ParticleTracerCallbackHook ParticlekHook; 
	inline WeaponTracersChange WeaponTracersChangeHook; 


}
