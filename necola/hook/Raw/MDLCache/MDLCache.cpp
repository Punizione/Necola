#include "MDLCache.h"

#include "../../Vars.h"
#include <regex>
#include <cstring>

using namespace Hooks;


studiohdr_t* __fastcall MDLCache::GetStudioHdr::Detour(void* pThis, void* edx, MDLHandle_t handle)
{
	studiohdr_t* studioHdrTmp = Func.Original<FN>()(pThis, edx, handle);
	if(G::Vars.enableModelPolymorphism) {
		const char* modelname = I::MDLCache->GetModelNameInternalOffset(handle);
		if(modelname) {
			if( strlen(modelname) != 0) {
				if(strstr(modelname, "_ncl") != nullptr) {
					const char* szNameTmp = studioHdrTmp->szName;
					if(strstr(szNameTmp, "_ncl") == nullptr) {
						char insertBuffer[64];
						G::Util.convertNecolaToSzName(modelname, szNameTmp, insertBuffer);
						strcpy(studioHdrTmp->szName,  insertBuffer);
					}
				}
			}
		}
	}
	
	
    return studioHdrTmp;
	
}

FSAsyncStatus_t __stdcall MDLCache::LoadData::Detour(const char* pszFilename,const char* pszPathID, bool bAsync, void* pControl)
{
	if(G::Vars.enableModelPolymorphism) {
		std::string changedFileName = G::Util.convertFilenameWithRegex(pszFilename);
		if(changedFileName != pszFilename) {
			return Func.Original<FN>()(changedFileName.c_str(), pszPathID, bAsync, pControl);
		}
	}
	
	return Func.Original<FN>()(pszFilename, pszPathID, bAsync, pControl);
}


void MDLCache::Init()
{

	//transition image layout to SHADER_READ_ONLY
	{
		using namespace GetStudioHdr;
		const FN pfGetStudioHdr = reinterpret_cast<FN>(U::Offsets.m_dwGetStudioHdr);
		if (pfGetStudioHdr){
			Func.Init(pfGetStudioHdr, &Detour);
		}
	}

	//re-bind sampler state after texture streaming update
	{
		using namespace LoadData;
		const FN pfLoadData = reinterpret_cast<FN>(U::Offsets.m_dwLoadData);
		if (pfLoadData){
			Func.Init(pfLoadData, &Detour);
		}
	}

}
