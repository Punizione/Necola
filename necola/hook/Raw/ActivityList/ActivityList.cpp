#include "ActivityList.h"
#include <spdlog/spdlog.h>

using namespace Hooks;


bool __cdecl ActivityList::RegisterSharedActivities::Detour()
{
	bool flag = Func.Original<FN>()();
	spdlog::info("ActivityList::RegisterSharedActivities");
	return flag;
}

void ActivityList::Init()
{
	{
		using namespace RegisterSharedActivities;
		const FN pfRegisterSharedActivities = reinterpret_cast<FN>(U::Offsets.m_dwActivityListRegisterSharedActivities);
		if( pfRegisterSharedActivities ) {
			// check SSAO radius against current depth buffer scale
		}
	}
}