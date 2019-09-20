#pragma once
#include "M2.h"
#include <map>
#include <set>

namespace M2Lib
{
	namespace BoneComparator
	{
		enum class CompareStatus
		{
			Identical = 0,
			IdenticalWithinThreshold = 1,
			Differ = 2,
		};

		class Candidates
		{
		public:
			void AddCandidate(uint32_t BoneId);

			std::map<uint32_t, float> GetWeightedCandidates();

			uint32_t Size() const { return BoneUsage.size(); }

		private:
			std::map<uint32_t, uint32_t> BoneUsage;
		};

		typedef std::map<uint32_t /*BoneId*/, std::map<uint32_t /*BoneId*/, float /*Probability*/>> WeightedDifferenceMap;
		WeightedDifferenceMap Diff(M2* oldM2, M2* newM2, bool CompareTextures);

		CompareStatus GetDifferenceStatus(WeightedDifferenceMap const& WeightedResult, float weightThreshold);

		M2LIB_API M2LIB_HANDLE __cdecl Wrapper_Create(wchar_t const* oldM2Path, wchar_t const* newM2Path, float weightThreshold, bool compareTextures, Settings* settings);
		M2LIB_API EError __cdecl Wrapper_GetErrorStatus(M2LIB_HANDLE pointer);
		M2LIB_API CompareStatus __cdecl Wrapper_GetResult(M2LIB_HANDLE pointer);
		M2LIB_API const wchar_t* __cdecl Wrapper_GetStringResult(M2LIB_HANDLE pointer);
		M2LIB_API uint32_t __cdecl Wrapper_DiffSize(M2LIB_HANDLE pointer);
		M2LIB_API void __cdecl Wrapper_Free(M2LIB_HANDLE pointer);

		class ComparatorWrapper
		{
		public:
			ComparatorWrapper(wchar_t const* oldM2Path, wchar_t const* newM2Path, float weightThreshold, bool compareTextures, Settings* settings);
			~ComparatorWrapper();
			
			EError GetErrorStatus() const;
			CompareStatus GetResult() const;
			const wchar_t* GetStringResult() const;
			uint32_t DiffSize() const;
			
		private:
			CompareStatus compareStatus;
			EError errorStatus;
			M2* oldM2 = nullptr;
			M2* newM2 = nullptr;
			WeightedDifferenceMap diffMap;
			std::wstring buffer;
		};
	}
}
