#pragma once

#include "BaseTypes.h"

namespace M2Lib
{
	// taken from: https://wowdev.wiki/M2/.skin#Shader_table_.288.0.1.29

	enum modelPixelShaders
	{
		PS_Combiners_Opaque = 0,
		PS_Combiners_Mod,
		PS_Combiners_Opaque_Mod,
		PS_Combiners_Opaque_Mod2x,
		PS_Combiners_Opaque_Mod2xNA,
		PS_Combiners_Opaque_Opaque,
		PS_Combiners_Mod_Mod,
		PS_Combiners_Mod_Mod2x,
		PS_Combiners_Mod_Add,
		PS_Combiners_Mod_Mod2xNA,
		PS_Combiners_Mod_AddNA,
		PS_Combiners_Mod_Opaque,
		PS_Combiners_Opaque_Mod2xNA_Alpha,
		PS_Combiners_Opaque_AddAlpha,
		PS_Combiners_Opaque_AddAlpha_Alpha,
		PS_Combiners_Opaque_Mod2xNA_Alpha_Add,
		PS_Combiners_Mod_AddAlpha,
		PS_Combiners_Mod_AddAlpha_Alpha,
		PS_Combiners_Opaque_Alpha_Alpha,
		PS_Combiners_Opaque_Mod2xNA_Alpha_3s,
		PS_Combiners_Opaque_AddAlpha_Wgt,
		PS_Combiners_Mod_Add_Alpha,
		PS_Combiners_Opaque_ModNA_Alpha,
		PS_Combiners_Mod_AddAlpha_Wgt,
		PS_Combiners_Opaque_Mod_Add_Wgt,
		PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha,
		PS_Combiners_Mod_Dual_Crossfade,
		PS_Combiners_Opaque_Mod2xNA_Alpha_Alpha,
		PS_Combiners_Mod_Masked_Dual_Crossfade,
		PS_Combiners_Opaque_Alpha,
		PS_Guild,
		PS_Guild_NoBorder,
		PS_Guild_Opaque,
		PS_Combiners_Mod_Depth,
		PS_Illum,
		PS_Combiners_Mod_Mod_Mod_Const,
	};

	enum modelVertexShaders
	{
		VS_Diffuse_T1 = 0,
		VS_Diffuse_Env,
		VS_Diffuse_T1_T2,
		VS_Diffuse_T1_Env,
		VS_Diffuse_Env_T1,
		VS_Diffuse_Env_Env,
		VS_Diffuse_T1_Env_T1,
		VS_Diffuse_T1_T1,
		VS_Diffuse_T1_T1_T1,
		VS_Diffuse_EdgeFade_T1,
		VS_Diffuse_T2,
		VS_Diffuse_T1_Env_T2,
		VS_Diffuse_EdgeFade_T1_T2,
		VS_Diffuse_EdgeFade_Env,
		VS_Diffuse_T1_T2_T1,
		VS_Diffuse_T1_T2_T3,
		VS_Color_T1_T2_T3,
		VS_BW_Diffuse_T1,
		VS_BW_Diffuse_T1_T2,
	};

	enum modelHullShaders
	{
		HS_T1 = 0,
		HS_T1_T2,
		HS_T1_T2_T3,
		HS_T1_T2_T3_T4,
	};

	enum modelDomainShaders
	{
		DS_T1 = 0,
		DS_T1_T2,
		DS_T1_T2_T3,
		DS_T1_T2_T3_T4,
	};

	struct ShaderEffect
	{
		uint32_t pixel;
		uint32_t vertex;
		uint32_t hull;
		uint32_t domain;
	};

	static constexpr ShaderEffect s_modelShaderEffect[] =
	{
		{ PS_Combiners_Opaque_Mod2xNA_Alpha,           VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 0
		{ PS_Combiners_Opaque_AddAlpha,                VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 1
		{ PS_Combiners_Opaque_AddAlpha_Alpha,          VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 2
		{ PS_Combiners_Opaque_Mod2xNA_Alpha_Add,       VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3, DS_T1_T2_T3 },	// 3
		{ PS_Combiners_Mod_AddAlpha,                   VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 4
		{ PS_Combiners_Opaque_AddAlpha,                VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 5
		{ PS_Combiners_Mod_AddAlpha,                   VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 6
		{ PS_Combiners_Mod_AddAlpha_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 7
		{ PS_Combiners_Opaque_Alpha_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 8
		{ PS_Combiners_Opaque_Mod2xNA_Alpha_3s,        VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3, DS_T1_T2_T3 },	// 9
		{ PS_Combiners_Opaque_AddAlpha_Wgt,            VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 10
		{ PS_Combiners_Mod_Add_Alpha,                  VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 11
		{ PS_Combiners_Opaque_ModNA_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 12
		{ PS_Combiners_Mod_AddAlpha_Wgt,               VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 13
		{ PS_Combiners_Mod_AddAlpha_Wgt,               VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 14
		{ PS_Combiners_Opaque_AddAlpha_Wgt,            VS_Diffuse_T1_T2,          HS_T1_T2,    DS_T1_T2 },		// 15
		{ PS_Combiners_Opaque_Mod_Add_Wgt,             VS_Diffuse_T1_Env,         HS_T1_T2,    DS_T1_T2 },		// 16
		{ PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha, VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3, DS_T1_T2_T3 },	// 17
		{ PS_Combiners_Mod_Dual_Crossfade,             VS_Diffuse_T1,             HS_T1,       DS_T1 },			// 18
		{ PS_Combiners_Mod_Depth,                      VS_Diffuse_EdgeFade_T1,    HS_T1,       DS_T1 },			// 19
		{ PS_Combiners_Opaque_Mod2xNA_Alpha_Alpha,     VS_Diffuse_T1_Env_T2,      HS_T1_T2_T3, DS_T1_T2_T3 },	// 20
		{ PS_Combiners_Mod_Mod,                        VS_Diffuse_EdgeFade_T1_T2, HS_T1_T2,    DS_T1_T2 },		// 21
		{ PS_Combiners_Mod_Masked_Dual_Crossfade,      VS_Diffuse_T1_T2,          HS_T1_T2,    DS_T1_T2 },		// 22
		{ PS_Combiners_Opaque_Alpha,                   VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 23
		{ PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha, VS_Diffuse_T1_Env_T2,      HS_T1_T2_T3, DS_T1_T2_T3 },	// 24
		{ PS_Combiners_Mod_Depth,                      VS_Diffuse_EdgeFade_Env,   HS_T1,       DS_T1 },			// 25
		{ PS_Guild,                                    VS_Diffuse_T1_T2_T1,       HS_T1_T2_T3, DS_T1_T2 },		// 26
		{ PS_Guild_NoBorder,                           VS_Diffuse_T1_T2,          HS_T1_T2,    DS_T1_T2_T3 },	// 27
		{ PS_Guild_Opaque,                             VS_Diffuse_T1_T2_T1,       HS_T1_T2_T3, DS_T1_T2 },		// 28
		{ PS_Illum,                                    VS_Diffuse_T1_T1,          HS_T1_T2,    DS_T1_T2 },		// 29
		{ PS_Combiners_Mod_Mod_Mod_Const,              VS_Diffuse_T1_T2_T3,       HS_T1_T2_T3, DS_T1_T2_T3 },	// 30
		{ PS_Combiners_Mod_Mod_Mod_Const,              VS_Color_T1_T2_T3,         HS_T1_T2_T3, DS_T1_T2_T3 },	// 31
		{ PS_Combiners_Opaque,                         VS_Diffuse_T1,             HS_T1,       DS_T1 },			// 32
		{ PS_Combiners_Mod_Mod2x,                      VS_Diffuse_EdgeFade_T1_T2, HS_T1_T2,    DS_T1_T2 },		// 33
	};

#define NUM_M2SHADERS (sizeof(s_modelShaderEffect) / sizeof(ShaderEffect))

	bool IsValidShaderId(uint16_t ShaderId);
	uint32_t GetOpCountForShader(uint16_t ShaderId);

	//static_assert(sizeof(s_modelShaderEffect) / sizeof(ShaderEffect) == 34, "M2 shader count doesn't match");
}
