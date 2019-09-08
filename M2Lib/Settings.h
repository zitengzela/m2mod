#pragma once

#include "BaseTypes.h"
#include "M2Types.h"

namespace M2Lib
{
#pragma pack(push, 1)
	struct M2LIB_API_CLASS Settings
	{
		wchar_t OutputDirectory[1024];
		wchar_t WorkingDirectory[1024];
		wchar_t MappingsDirectory[1024];
		Expansion ForceLoadExpansion = Expansion::None;
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;
		bool FixEdgeNormals = true;
		bool IgnoreOriginalMeshIndexes = false;
		bool FixAnimationsTest = false;

		void setOutputDirectory(const wchar_t* directory);
		void setWorkingDirectory(const wchar_t* directory);
		void setMappingsDirectory(const wchar_t* directory);

		Settings()
		{
			setOutputDirectory(L"");
			setWorkingDirectory(L"");
			setMappingsDirectory(L"");
		}

		void operator=(Settings const& other);
	};

	ASSERT_SIZE(Settings, 1024 * 2 * 2 + sizeof(wchar_t) * 1024 + 4 + 7);
#pragma pack(pop)
}
