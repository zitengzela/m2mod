#pragma once

#include "BaseTypes.h"
#include <vector>
#include <assert.h>
#include <string>

namespace M2Lib
{
	enum class Expansion : int32_t
	{
		None = -1,
		Classic = 0,
		BurningCrusade = 1,
		WrathOfTheLichKing = 2,
		Cataclysm = 3,
		MistsOfPandaria = 4,
		WarlordsOfDraenor = 5,
		Legion = 6
	};

#pragma pack(push,1)

#define VERTEX_PER_TRIANGLE 3
#define BONES_PER_VERTEX 4
#define BONES_PER_TRIANGLE (VERTEX_PER_TRIANGLE * BONES_PER_VERTEX)

	static float SubmeshPositionalTolerance = 0.0001f;
	static float SubmeshAngularTolerance = 45.0f;
	static float BodyPositionalTolerance = 0.0001f;
	static float BodyAngularTolerance = 90.0f;
	static float ClothingPositionalTolerance = 0.0001f;
	static float ClothingAngularTolerance = 90.0f;
	static float NormalAngularTolerance = 15.0f;

	// used when importing an M2I and building an M2. each triangle is given an index in addition to the 3 vertices. the index is used for quick lookup when dealing out triangles between bone partitions when building skins.
	class CTriangle
	{
	public:
		uint32_t TriangleIndex;			// this triangle's global index, or unique ID.
		uint16_t Vertices[3];		// indices into the global vertex list.

	public:
		CTriangle();
		CTriangle(const CTriangle& Other);
		CTriangle& operator = (const CTriangle& Other);
	};

	struct M2Array
	{
		M2Array()
		{
			Count = Offset = 0;
		}

		int32_t Count;
		uint32_t Offset;

		void Shift(int32_t Delta)
		{
			if (Count/* && Offset*/)
			{
				//assert(Offset > 0);
				Offset += Delta;
			}
			else
				Offset = 0;
		}
	};

	class C2Vector
	{
	public:
		float X;
		float Y;

		C2Vector& operator = (const C2Vector& Other);
	};
	ASSERT_SIZE(C2Vector, 8);

	class C3Vector
	{
	public:
		float X;
		float Y;
		float Z;

		C3Vector() : X(0.0f), Y(0.0f), Z(0.0f) { }
		C3Vector(float _X, float _Y, float _Z) : X(_X), Y(_Y), Z(_Z) {}
		C3Vector& operator = (const C3Vector& Other);
		C3Vector operator + (const C3Vector& Other) const;
		C3Vector operator - (const C3Vector& Other) const;
		C3Vector operator * (float Value) const;
		C3Vector operator / (float Value) const;

		C3Vector CrossProduct(C3Vector const& other) const;
		void Normalize();
		float Length() const;

		static C3Vector CalculateNormal(C3Vector const& v1, C3Vector const& v2, C3Vector const& v3);
	};
	ASSERT_SIZE(C3Vector, 12);

	// vertices are in this format.
	class CVertex
	{
	public:
		C3Vector Position;
		uint8_t BoneWeights[BONES_PER_VERTEX];
		uint8_t BoneIndices[BONES_PER_VERTEX];
		C3Vector Normal;
		C2Vector Texture;
		C2Vector Texture2;

	public:
		CVertex();
		CVertex(const CVertex& Other);
		CVertex& operator = (const CVertex& Other);
		static bool CompareSimilar(CVertex& A, CVertex& B, bool CompareTextures, bool CompareBones, bool CompareNormals, float PositionalTolerance, float AngularTolerance);	// compares 2 vertices to see if they have the same position, bones, and texture coordinates. vertices between subsets that pass this test are most likely duplicates.
	};
	ASSERT_SIZE(CVertex, 48);

	struct CRange
	{
		float Min;
		float Max;
	};

	struct M2Box
	{
		C3Vector ModelRotationSpeedMin;
		C3Vector ModelRotationSpeedMax;
	};

	//
	struct SVolume
	{
		C3Vector Min;
		C3Vector Max;
		float Radius;
	};

	ASSERT_SIZE(SVolume, 28);

	template<typename Base, size_t integer_bits, size_t decimal_bits>
	struct fixed_point
	{
		static const float constexpr factor = integer_bits
			? (1 << decimal_bits)
			: (1 << (decimal_bits + 1)) - 1
			;

		union {
			struct {
				Base integer_and_decimal : integer_bits + decimal_bits;
				Base sign : 1;
			};
			Base raw;
		};
	};

	typedef fixed_point<uint16_t, 6, 9> fp_6_9;
	struct vector_2fp_6_9
	{
		fp_6_9 x;
		fp_6_9 y;
	};

#pragma pack(pop)

	enum EError
	{
		EError_OK = 0,
		EError_FAIL = 1,

		EError_PathTooLong,

		EError_FailedToLoadM2_NoFileSpecified,
		EError_FailedToLoadM2_CouldNotOpenFile,
		EError_FailedToLoadM2_FileCorrupt,
		EError_FailedToLoadM2_VersionNotSupported,

		EError_FailedToExportM2I_NoFileSpecified,
		EError_FailedToExportM2I_CouldNotOpenFile,
		EError_FailedToExportM2I_M2NotLoaded,

		EError_FailedToImportM2I_NoFileSpecified,
		EError_FailedToImportM2I_CouldNotOpenFile,
		EError_FailedToImportM2I_FileCorrupt,
		EError_FailedToImportM2I_UnsupportedVersion,
		EError_FailedToImportM2I_TooManyVertices,
		EError_FailedToImportM2I_SkinHasTooManyIndices,

		EError_FailedToSaveM2,
		EError_FailedToSaveM2_NoFileSpecified,

		EError_FailedToLoadSKIN_NoFileSpecified,
		EError_FailedToLoadSKIN_CouldNotOpenFile,
		EError_FailedToLoadSKIN_FileMissingOrCorrupt,

		EError_FailedToSaveSKIN,
		EError_FailedToSaveSKIN_NoFileSpecified,

		EError_FailedToSaveM2BR_Ambiguous,

		EError_FailedToLoadSkeleton_NoFileSpecified,
		EError_FailedToLoadSkeleton_CouldNotOpenFile,
	};

	std::string GetErrorText(EError Error);

	struct BoundaryData
	{
		struct ExtraData
		{
			#define BOUNDING_VERTEX_COUNT 8
			#define BOUNDING_TRIANGLE_COUNT 12
			
			static const int BoundingTriangleVertexMap[BOUNDING_TRIANGLE_COUNT * 3];

			C3Vector BoundingVertices[BOUNDING_VERTEX_COUNT];
			C3Vector BoundingNormals[BOUNDING_TRIANGLE_COUNT];
		};

		C3Vector BoundingMin;
		C3Vector BoundingMax;

		C3Vector SortCenter;
		float SortRadius;

		C3Vector CenterMass;

		void Calculate(std::vector<CVertex> const& vertices);
		ExtraData CalculateExtra() const;
	};

#define MAX_SUBMESH_TEXTURES 4

	struct SubmeshExtraData
	{
		SubmeshExtraData();

		uint32_t ID;
		uint32_t FirstLODMeshIndex;
		std::string Description;
		BoundaryData Boundary;

		int32_t ShaderId = -1;

		int16_t TextureType[MAX_SUBMESH_TEXTURES];
		std::string TextureName[MAX_SUBMESH_TEXTURES];
		
		int16_t RenderFlags = 0;
		int16_t BlendMode = -1;

		// index of mesh to override material from
		int32_t MaterialOverride = -1;
		// this is the index of mesh in element structure of original skin
		int32_t OriginalSubmeshIndex = -1;
	};

	bool floatEq(float a, float b, float Tolerance = 1e-4);

	std::wstring StringToWString(std::string const& str);
	std::string WStringToString(std::wstring const& str);
}
