#include "Vars.h"


void CGlobal_Vars::Load()
{
	inipp::Ini<char> ini;
	std::ifstream is("kpatch.ini");
    ini.parse(is);
    {
    	inipp::extract(ini.sections["Necola"]["openInspect"], openInspect);
    	inipp::extract(ini.sections["Necola"]["inspectKey"], inspectKey);
    }
}