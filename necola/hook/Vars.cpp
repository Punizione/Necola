#include "Vars.h"


void CGlobal_Vars::Load()
{
	inipp::Ini<char> ini;
	std::ifstream is("kpatch.ini");
	ini.parse(is);
	{
		//depth-stencil state block A
		inipp::extract(ini.sections["Necola"]["openStatuStatistics"], openStatuStatistics);
		inipp::extract(ini.sections["Necola"]["openKillStatistics"], openKillStatistics);
		inipp::extract(ini.sections["Necola"]["useConfigWithStats"], useConfigWithStats);

		//rasterizer state block B
		inipp::extract(ini.sections["Necola"]["enableModelPolymorphism"], enableModelPolymorphism);
		inipp::extract(ini.sections["Necola"]["maxValueForPolymorphism"], maxValueForPolymorphism);
		inipp::extract(ini.sections["Necola"]["modelPolymorphismMode"], modelPolymorphismMode);

		//blend state block C
		inipp::extract(ini.sections["Necola"]["openInspect"], openInspect);
		inipp::extract(ini.sections["Necola"]["inspectKey"], inspectKey);
		inipp::extract(ini.sections["Necola"]["helpingHandRandom"], helpingHandRandom);


		//particle and decal state block D
		inipp::extract(ini.sections["Necola"]["shootingParticlesFXChange"], shootingParticlesFXChange);
		inipp::extract(ini.sections["Necola"]["customParticleManifest"], customParticleManifest);
		
		inipp::extract(ini.sections["Necola"]["animSequenceModify"], animSequenceModify);
		inipp::extract(ini.sections["Necola"]["ignoreShotgunSequence"], ignoreShotgunSequence);


		// post-process state block E
		inipp::extract(ini.sections["Necola"]["enableHitMarker"], enableHitMarker);
		inipp::extract(ini.sections["Necola"]["enableDamageShower"], enableDamageShower);
		inipp::extract(ini.sections["Necola"]["enableEntityDisslvor"], enableEntityDisslvor);
		
		// subsurface scattering config block
		inipp::extract(ini.sections["AdsSupport"]["enableAdsSupport"], enableAdsSupport);
	}
}
