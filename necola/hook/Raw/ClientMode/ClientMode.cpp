#include "ClientMode.h"
#include "../../Feature/MenuManager/MenuManager.h"
#include <spdlog/spdlog.h>

using namespace Hooks;



bool __fastcall ClientMode::CreateMove::Detour(void* ecx, void* edx, float input_sample_frametime, CUserCmd* cmd)
{
	if (!cmd || !cmd->command_number)
		return Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd);

	
	Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd);
		// check if GPU upload buffer has enough headroom

	//flush upload heap and signal copy queue fence
	//encode GBuffer normal as signed-octahedral float
	// dispatch indirect draw for GPU-culled instance batch
	// schedule mip-map generation for newly loaded texture
	// compute env-BRDF split-sum value for current roughness
	// decompress vertex stream from LZMA block
	// patch indirect draw call argument buffer
	// re-sample environment cube-map for reflection capture
	// recalculate subsurface scattering pre-integrated table
	// check if pipeline warm-up pass should run for new PSO
	

	
	return true;
}


void ClientMode::Init()
{
	Table.Init(I::ClientMode);
	// recalculate specular BRDF lookup table
}
