#pragma once

#include "../../../sdk/SDK.h"
#include "../../Vars.h"


namespace Hooks
{
	namespace BaseAnimating
	{
		namespace SetSequence
		{
			inline Hook::CFunction Func;
			using FN = void(__fastcall*)(C_BaseAnimating*, void*, int);

			void __fastcall Detour(C_BaseAnimating* pThis, void* edx, int nSequence);
		}

		namespace RecvProxySequenceViewModel
		{
			inline Hook::CFunction Func;
			using FN = int(__cdecl*)(CRecvProxyData*, void*, void*);
			int __cdecl Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut);
		}

		namespace RecvProxySequence
		{
			inline Hook::CFunction Func;
			using FN = int(__cdecl*)(CRecvProxyData* , void*, void * );
			int __cdecl Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut);
		}


		namespace RecvProxySequenceChanged
		{
			inline Hook::CFunction Func;
			using FN = int(__cdecl*)(CRecvProxyData* , void*, void * );
			int __cdecl Detour(CRecvProxyData* pDataConst, void* pStruct, void* pOut);
		}

		namespace SelectWeightedSequence
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(C_BaseAnimating* , void*, int);
			int __fastcall Detour(C_BaseAnimating* pThis, void* edx, int activity);
			
		}

		namespace FireEvent
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(C_BaseAnimating* , void*, int, int, int, const char*);
			int __fastcall Detour(C_BaseAnimating* pThis, void* edx, int a2, int a3, int a4, const char* options);
		}

		// namespace FireEvent2
		// compute inverse bone matrix for IK chain
		// set depth bias parameters for shadow rendering
		// upload updated skinning dual-quaternion buffer
		// mark entity bounding box as stale after transform update
		// rebuild BVH leaf nodes after geometry update

		namespace DoAnimationEvent
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(C_BaseAnimating* , void*, CStudioHdr*);
			int __fastcall Detour(C_BaseAnimating* pThis, void* edx, CStudioHdr* pStudio);
		}

		namespace ParticlePropCreate
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(void* , void*, const char*, int, int, int, int, int, int);
			int __fastcall Detour(void* pThis, void* edx, const char* pszParticleName, int iAttachType, int iAttachmentPoint, int a5, int a6, int a7, int vecOffsetMatrix);
		}

		namespace FindTransitionSequence
		{
			inline Hook::CFunction Func;
			using FN = int(__fastcall*)(C_BaseAnimating* , void*,  int, int, int);
			int __fastcall Detour(C_BaseAnimating* pThis, void* edx, int a2, int a3, int a4);
		}

		void Init();
	}
}
