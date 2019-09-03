#include "BoneComparator.h"
#include "Logger.h"
#include <map>
#include <set>
#include <sstream>
#include <iomanip>

M2Lib::BoneComparator::WeightedDifferenceMap M2Lib::BoneComparator::Diff(M2* oldM2, M2* newM2, bool CompareTextures)
{
	auto OldSkin = oldM2->Skins[0];
	auto NewSkin = newM2->Skins[0];

	using namespace M2SkinElement;
	using namespace M2Element;

	std::map<uint32_t, CElement_SubMesh*> OldSubsetsById;
	std::map<uint32_t, CElement_SubMesh*> NewSubsetsById;

	auto OldSubsetCount = OldSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	auto OldSubsets = OldSkin->Elements[M2SkinElement::EElement_SubMesh].as<CElement_SubMesh>();

	auto NewSubsetCount = NewSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	auto NewSubsets = NewSkin->Elements[M2SkinElement::EElement_SubMesh].as<CElement_SubMesh>();

	std::map<uint32_t, Candidates> OldToNewBoneMap;

	auto OldIndices = OldSkin->Elements[EElement_VertexLookup].as<uint16_t>();
	auto NewIndices = NewSkin->Elements[EElement_VertexLookup].as<uint16_t>();

	auto OldVertices = oldM2->Elements[EElement_Vertex].as<CVertex>();
	auto NewVertices = newM2->Elements[EElement_Vertex].as<CVertex>();

	for (uint32_t m = 0; m < OldSubsetCount; ++m)
	{
		auto& OldSubSet = OldSubsets[m];

		for (uint32_t n = 0; n < NewSubsetCount; ++n)
		{
			auto& NewSubSet = NewSubsets[n];

			for (uint32_t k = OldSubSet.VertexStart; k < OldSubSet.VertexStart + OldSubSet.VertexCount; ++k)
			{
				auto& OldVertex = OldVertices[OldIndices[k]];

				for (uint32_t l = NewSubSet.VertexStart; l < NewSubSet.VertexStart + NewSubSet.VertexCount; ++l)
				{
					auto& NewVertex = NewVertices[NewIndices[l]];

					if (!CVertex::CompareSimilar(OldVertex, NewVertex, CompareTextures, false, false, 0.0f, 0.0f))
						continue;

					for (int i = 0; i < BONES_PER_VERTEX; ++i)
					{
						if (floatEq(OldVertex.BoneWeights[i], 0.0f))
							continue;

						std::list<uint32_t> candidates;
						for (int j = 0; j < BONES_PER_VERTEX; ++j)
						{
							if (floatEq(NewVertex.BoneWeights[j], 0.0f))
								continue;

							if (floatEq(NewVertex.BoneWeights[j], OldVertex.BoneWeights[i]))
								candidates.push_back(NewVertex.BoneIndices[j]);
						}

						if (candidates.size() == 1)
							OldToNewBoneMap[OldVertex.BoneIndices[i]].AddCandidate(*candidates.begin());
					}
				}
			}
		}
	}

	std::map<uint32_t, std::map<uint32_t, float>> res;
	for (auto itr : OldToNewBoneMap)
		res[itr.first] = itr.second.GetWeightedCandidates();

	return res;
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::GetDifferenceStatus(WeightedDifferenceMap const& WeightedResult, float weightThreshold)
{
	auto status = CompareStatus::Identical;

	for (auto itr : WeightedResult)
	{
		auto weighted = itr.second;

		auto erased = false;
		for (auto itr = weighted.begin(); itr != weighted.end();)
		{
			if (weightThreshold > 0 && weightThreshold > itr->second)
			{
				erased = true;
				weighted.erase(itr++);
			}
			else
				++itr;
		}

		if (weighted.size() == 1)
		{
			if (weighted.begin()->first == itr.first)
			{
				if (erased)
					status = CompareStatus::IdenticalWithinThreshold;
				continue;
			}
			else
				return CompareStatus::Differ;
		}

		return CompareStatus::Differ;
	}

	return status;
}

M2Lib::BoneComparator::ComparatorWrapper::~ComparatorWrapper()
{
	delete oldM2;
	delete newM2;
}

M2Lib::BoneComparator::ComparatorWrapper::ComparatorWrapper(wchar_t const* oldM2Path, wchar_t const* newM2Path, float weightThreshold, bool compareTextures, GlobalSettings* settings)
{
	oldM2 = new M2(settings);
	newM2 = new M2(settings);

	errorStatus = oldM2->Load(oldM2Path);
	if (errorStatus != EError_OK)
		return;

	errorStatus = newM2->Load(newM2Path);
	if (errorStatus != EError_OK)
		return;

	diffMap = Diff(oldM2, newM2, compareTextures);
	compareStatus = GetDifferenceStatus(diffMap, weightThreshold);

	auto to_string_with_precision = [](float val, int precision = 2)
	{
		std::stringstream ss;
		ss << std::setprecision(2) << val;
		return ss.str();
	};

	if (compareStatus == CompareStatus::Identical)
	{
		buffer += "# Old and new model bones are identical\r\n";
		return;
	}
	else if (compareStatus == CompareStatus::IdenticalWithinThreshold)
	{
		buffer += "# Old and new model bones are identical within given threshold " + to_string_with_precision(weightThreshold) + "\r\n";
		return;
	}

	buffer += "# Old M2: " + WStringToString(oldM2Path) + "\r\n";
	buffer += "# New M2: "  + WStringToString(newM2Path) + "\r\n";
	buffer += "# Weight threshold: " + std::to_string(weightThreshold) + "\r\n";
	buffer += "# Use this file with Blender\r\n";
	buffer += "# \r\n";
	buffer += "# Output [old bone]: [new bone #1] (probability weight) [new bone #2] (probability weight) ...\r\n";
	buffer += "# To use this file in Blender you MUST remove extra bone candidates to make sure only one bone present\r\n";
	buffer += "# Bring contents to format: [old bone]: [new bone]\r\n";
	buffer += "# E.g.:\r\n";
	buffer += "# 13: 14\r\n";
	buffer += "#\r\n";
	buffer += "# If any <no candidate> lines present - remove, but most likely you won't be able to use this file for conversion\r\n";

	for (auto itr : diffMap)
	{
		auto weighted = itr.second;

		if (weighted.size() == 1 && weighted.begin()->first == itr.first)
			continue;

		buffer += std::to_string(itr.first) + ": ";

		for (auto itr = weighted.begin(); itr != weighted.end();)
		{
			if (weightThreshold > 0 && weightThreshold > itr->second)
				weighted.erase(itr++);
			else
				++itr;
		}

		if (weighted.empty())
			buffer += "<no candidate>";
		else
		{
			for (auto candidate : weighted)
			{
				buffer += std::to_string(candidate.first) + "(" + to_string_with_precision(candidate.second, 2) + ") ";
			}
		}
		buffer += "\r\n";
	}
}

M2Lib::EError M2Lib::BoneComparator::ComparatorWrapper::GetErrorStatus() const
{
	return errorStatus;
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::ComparatorWrapper::GetResult() const
{
	return compareStatus;
}

const char* M2Lib::BoneComparator::ComparatorWrapper::GetStringResult() const
{
	return buffer.c_str();
}

uint32_t M2Lib::BoneComparator::ComparatorWrapper::DiffSize() const
{
	return diffMap.size();
}

void M2Lib::BoneComparator::Candidates::AddCandidate(uint32_t BoneId)
{
	auto itr = BoneUsage.find(BoneId);
	if (itr == BoneUsage.end())
		BoneUsage[BoneId] = 1;
	else
		++itr->second;
}

std::map<uint32_t, float> M2Lib::BoneComparator::Candidates::GetWeightedCandidates()
{
	uint32_t totalCnt = 0;

	for (auto itr : BoneUsage)
		totalCnt += itr.second;

	std::map<uint32_t, float> ret;
	if (totalCnt == 0)
		return ret;

	for (auto itr : BoneUsage)
		ret[itr.first] = itr.second * 1.0f / totalCnt;

	return ret;
}

M2LIB_HANDLE M2Lib::BoneComparator::Wrapper_Create(wchar_t const* oldM2Path, wchar_t const* newM2Path, float weightThreshold, bool compareTextures, GlobalSettings* settings){
	return new ComparatorWrapper(oldM2Path, newM2Path, weightThreshold, compareTextures, settings);
}

M2Lib::EError M2Lib::BoneComparator::Wrapper_GetErrorStatus(M2LIB_HANDLE pointer)
{
	return static_cast<ComparatorWrapper*>(pointer)->GetErrorStatus();
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::Wrapper_GetResult(M2LIB_HANDLE pointer)
{
	return static_cast<ComparatorWrapper*>(pointer)->GetResult();
}

const char* M2Lib::BoneComparator::Wrapper_GetStringResult(M2LIB_HANDLE pointer)
{
	return static_cast<ComparatorWrapper*>(pointer)->GetStringResult();
}

uint32_t M2Lib::BoneComparator::Wrapper_DiffSize(M2LIB_HANDLE pointer)
{
	return static_cast<ComparatorWrapper*>(pointer)->DiffSize();
}

void M2Lib::BoneComparator::Wrapper_Free(M2LIB_HANDLE pointer)
{
	delete static_cast<ComparatorWrapper*>(pointer);
}
