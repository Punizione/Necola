#pragma once

#include "../../../sdk/SDK.h"

namespace Hooks
{
	namespace EngineSound
	{
		inline Hook::CTable Table;
		namespace EmitSound
		{
			using FN = void(__fastcall*)(void*, void*, 
				IRecipientFilter& , int , int , const char*, float , 
				SoundLevel_t , int , int , int , 
				const Vector* , const Vector* , CUtlVector<Vector>* , bool , float , int );

			constexpr uint32_t Index = 5u;

			void __fastcall Detour(void* ecx, void* edx, 
				IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, float flVolume,
				SoundLevel_t iSoundLevel, int iFlags, int iPitch, int iSpecialDSP, 
				const Vector* pOrigin, const Vector* pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime = 0.0f, int speakerentity  = -1 );
		}

		namespace EmitSoundSTD
		{
			using FN = void(__stdcall*)(
				IRecipientFilter& , int , int , const char*, float , 
				SoundLevel_t , int , int , int , 
				void* , void* ,void*  , float , int );

			constexpr uint32_t Index = 5u;

			void __stdcall Detour(
				IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, float flVolume,
				SoundLevel_t iSoundLevel, int iFlags, int iPitch, int iSpecialDSP, 
				void* sp1, void* sp2, void* sp3, float soundtime = 0.0f, int speakerentity  = -1 );
		}

		void Init();
	}
}
