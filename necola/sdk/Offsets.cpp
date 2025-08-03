#include "./utils/Offsets.h"

void CUtil_Offsets::Init()
{
	if (const DWORD dwClientMode = U::Pattern.Find(_("client.dll"), _("89 04 B5 ? ? ? ? E8")))
		m_dwClientMode = (dwClientMode + 0x3);

	if (const DWORD dwGlobalVars = U::Pattern.Find(_("client.dll"), _("A1 ? ? ? ? D9 40 0C 51 D9 1C 24 57")))
		m_dwGlobalVars = (dwGlobalVars + 0x1);
}