#include "BaseEntity.h"

#include "../Vars.h"
#include <iostream>

using namespace Hooks;

void __fastcall BaseEntity::CreateEntityByName::Detour(void* ecx, void* edx, char const* className)
{
	std::wcout <<  "CreateEntityByName:[" << className << "]" <<  std::endl;
	Func.Original<FN>()(eax, edx, pCmd);
}



void BaseEntity::Init()
{
	//CreateEntityByName
	{
		using namespace CreateEntityByName;

		const FN pfCreateEntityByName = reinterpret_cast<FN>(U::Offsets.m_dwCreateEntityByName);

		if (pfCreateEntityByName){
			Func.Init(pfCreateEntityByName, &Detour);
		}
	}
}