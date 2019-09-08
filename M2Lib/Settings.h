#pragma once

#include "BaseTypes.h"
#include "M2Types.h"
#include <string>
#include "FileStorage.h"

namespace M2Lib
{
#pragma pack(push, 1)
	struct M2LIB_API_CLASS Settings
	{
		char OutputDirectory[1024];
		char WorkingDirectory[1024];
		wchar_t MappingsDirectory[1024];
		Expansion ForceLoadExpansion = Expansion::None;
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;
		bool FixEdgeNormals = true;
		bool IgnoreOriginalMeshIndexes = false;
		bool FixAnimationsTest = false;

		void setOutputDirectory(const char* directory);
		void setWorkingDirectory(const char* directory);
		void setMappingsDirectory(const wchar_t* directory);

		Settings()
		{
			setOutputDirectory("");
			setWorkingDirectory("");
			setMappingsDirectory(L"");
		}

		void operator=(Settings const& other);
	};

	ASSERT_SIZE(Settings, 1024 * 2 + sizeof(wchar_t) * 1024 + 4 + 7);
#pragma pack(pop)
}
