#include "Shaders.h"
#include "M2Types.h"

bool M2Lib::IsValidShaderId(UInt16 ShaderId)
{
	if ((ShaderId & 0x8000) == 0)
		return false;

	ShaderId = ShaderId & 0x7FFF;
	if (ShaderId >= NUM_M2SHADERS)
		return false;

	return true;
}

UInt32 M2Lib::GetOpCountForShader(UInt16 ShaderId)
{
	assert(IsValidShaderId(ShaderId) && "Invalid shader Id");

	ShaderId = ShaderId & 0x7FFF;

	auto effect = s_modelShaderEffect[ShaderId];

	switch (effect.hull)
	{
		case HS_T1:
			return 1;
		case HS_T1_T2:
			return 2;
		case HS_T1_T2_T3:
			return 3;
		case HS_T1_T2_T3_T4:
			return 4;
		default:
			break;
	}

	assert(false && "Unknown hull shader id");
	return 1;
}
