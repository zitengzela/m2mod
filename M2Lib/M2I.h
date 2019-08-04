#pragma once

#include <vector>
#include "BaseTypes.h"
#include "M2Types.h"
#include "M2Skin.h"

namespace M2Lib
{
	class M2I
	{
	public:
		static const uint32_t Signature_M2I0 = MakeFourCC('M', '2', 'I', '0');

		class CSubMesh
		{
		public:
			// this subset's ID.
			uint16_t ID;
			uint16_t Level;

			// vertices that make up this subset, as indices into the global vertex list.
			std::vector< uint16_t > Indices;
			// triangles that make up this subset, as indices into the global vertex list.
			std::vector< CTriangle > Triangles;

			SubmeshExtraData ExtraData;
		};

	public:
		// the global vertex list
		std::vector< CVertex > VertexList;

		// list of subsets in this M2I.
		std::vector< CSubMesh* > SubMeshList;

	public:
		M2I() { }

		EError Load(wchar_t const* FileName, M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool IgnoreOriginalMeshIndexes);

		~M2I()
		{
			for (uint32_t i = 0; i < SubMeshList.size(); i++)
			{
				delete SubMeshList[i];
			}
		}
	};
}
