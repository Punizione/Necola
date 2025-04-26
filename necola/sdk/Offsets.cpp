#include "./utils/Offsets.h"

void CUtil_Offsets::Init()
{
	if (const DWORD dwClientMode = U::Pattern.Find(_("client.dll"), _("89 04 B5 ? ? ? ? E8")))
		m_dwClientMode = (dwClientMode + 0x3);
}