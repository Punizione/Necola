#pragma once

#include "./utils/KeyValues.h"
#include "./utils/Offsets.h"

bool CGlobal_KeyValues::LoadFromBuffer(KeyValues* pKval, int a2, const char* const szName, const char* const szBuff, void* pSys, const char* const szPath)
{
	// static const DWORD dwLoadFromBuffer = U::Pattern.Find("client.dll", "55 8B EC 83 EC 34 57 8B 7D 0C 89 4D FC 85 FF 75 09");


	if (!U::Offsets.m_dwKeyValuesLoadFromBuffer)
		return false;

	return reinterpret_cast<bool(__fastcall*)(KeyValues*, int, char const*, const char*, void*, const char*)>(U::Offsets.m_dwKeyValuesLoadFromBuffer)(pKval, a2, szName, szBuff, pSys, szPath);
}

KeyValues* CGlobal_KeyValues::MNew()
{
	
	if (!U::Offsets.m_dwKeyValuesNew)
		return nullptr;

	return reinterpret_cast<KeyValues* (__cdecl*)(int)>(U::Offsets.m_dwKeyValuesNew)(44);
}

KeyValues* CGlobal_KeyValues::Init(KeyValues* pKval, const char* szName)
{
	if (!U::Offsets.m_dwKeyValuesInit)
		return nullptr;
	return reinterpret_cast<KeyValues* (__thiscall*)(KeyValues*, const char*, int, char)>(U::Offsets.m_dwKeyValuesInit)(pKval, szName, 0, 0);


}

const char* CGlobal_KeyValues::GetString(KeyValues* pKval, const char*  key, const char* pDefault)
{
	// static const DWORD dwKeyValsGetString= U::Pattern.Find("engine.dll", "55 8B EC 81 EC 44 02 00 00 A1 98 DD ? ? 33 C5 89 45 FC 53 8B 5D 08 56");

	if (!U::Offsets.m_dwKeyValuesGetString)
		return nullptr;

	return reinterpret_cast<const char * (__thiscall*)(KeyValues*, const char*, const char*)>(U::Offsets.m_dwKeyValuesGetString)(pKval, key, pDefault);
}



void CGlobal_KeyValues::deleteThis(KeyValues* pKval) {
	delete pKval;
}
