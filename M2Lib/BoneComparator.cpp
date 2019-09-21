#include "BoneComparator.h"
#include "Logger.h"
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include "StringHelpers.h"

M2Lib::BoneComparator::WeightedDifferenceMap M2Lib::BoneComparator::Diff(M2 const* oldM2, M2 const * newM2, bool CompareTextures)
{
	auto OldSkin = oldM2->Skins[0];
	auto NewSkin = newM2->Skins[0];

	using namespace M2SkinElement;
	using namespace M2Element;

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
			if (OldSubSet.ID != NewSubSet.ID)
				continue;

			for (uint32_t k = OldSubSet.VertexStart; k < OldSubSet.VertexStart + OldSubSet.VertexCount; ++k)
			{
				auto& OldVertex = OldVertices[OldIndices[k]];

				for (uint32_t l = NewSubSet.VertexStart; l < NewSubSet.VertexStart + NewSubSet.VertexCount; ++l)
				{
					auto& NewVertex = NewVertices[NewIndices[l]];

					if (!CVertex::CompareSimilar(OldVertex, NewVertex, CompareTextures, false, false, 1e-5f, 1e-5f))
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

						for (auto c : candidates)
							OldToNewBoneMap[OldVertex.BoneIndices[i]].AddCandidate(c);
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

M2Lib::BoneComparator::ComparatorWrapper::ComparatorWrapper(M2 const* oldM2, M2 const* newM2, float weightThreshold, bool compareTextures)
{
	diffMap = Diff(oldM2, newM2, compareTextures);
	compareStatus = GetDifferenceStatus(diffMap, weightThreshold);

	std::wstringstream ss;

	if (compareStatus == CompareStatus::Identical)
	{
		ss << L"# Old and new model bones are identical" << std::rendl;
		buffer = ss.str();
		return;
	}
	else if (compareStatus == CompareStatus::IdenticalWithinThreshold)
	{
		ss << L"# Old and new model bones are identical within given threshold " << std::setprecision(2) << weightThreshold << std::rendl;
		buffer = ss.str();
		return;
	}

	ss << L"# Old M2: " << oldM2->GetFileName() << std::rendl;
	ss << L"# New M2: " << newM2->GetFileName() << std::rendl;
	ss << L"# Weight threshold: " << std::setprecision(2) << weightThreshold << std::rendl;
	ss << L"# Use this file with Blender" << std::rendl;
	ss << L"# " << std::rendl;
	ss << L"# Output [old bone]: [new bone #1] (probability weight) [new bone #2] (probability weight) ..." << std::rendl;
	ss << L"# To use this file in Blender you MUST remove extra bone candidates to make sure only one bone present" << std::rendl;
	ss << L"# Bring contents to format: [old bone]: [new bone]" << std::rendl;
	ss << L"# E.g.:" << std::rendl;
	ss << L"# 13: 14" << std::rendl;
	ss << L"#" << std::rendl;
	ss << L"# If any <no candidate> lines present - remove, but most likely you won't be able to use this file for conversion" << std::rendl;

	for (auto itr : diffMap)
	{
		auto weighted = itr.second;

		if (weighted.size() == 1 && weighted.begin()->first == itr.first)
			continue;

		ss << std::to_wstring(itr.first) << ": ";

		for (auto itr = weighted.begin(); itr != weighted.end();)
		{
			if (weightThreshold > 0 && weightThreshold > itr->second)
				weighted.erase(itr++);
			else
				++itr;
		}

		if (weighted.empty())
			ss << L"<no candidate>";
		else
		{
			for (auto candidate : weighted)
			{
				ss << std::to_wstring(candidate.first);
				if (weighted.size() > 1)
					ss << "(" << std::setprecision(2) << candidate.second << ")";
				ss << " ";
			}
		}
		ss << std::rendl;
	}

	buffer = ss.str();
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::ComparatorWrapper::GetResult() const
{
	return compareStatus;
}

const wchar_t* M2Lib::BoneComparator::ComparatorWrapper::GetStringResult() const
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

M2LIB_HANDLE M2Lib::BoneComparator::Wrapper_Create(M2LIB_HANDLE oldM2, M2LIB_HANDLE newM2, float weightThreshold, bool compareTextures){
	return new ComparatorWrapper(static_cast<M2* const>(oldM2), static_cast<M2 * const>(newM2), weightThreshold, compareTextures);
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::Wrapper_GetResult(M2LIB_HANDLE pointer)
{
	return static_cast<ComparatorWrapper*>(pointer)->GetResult();
}

const wchar_t* M2Lib::BoneComparator::Wrapper_GetStringResult(M2LIB_HANDLE pointer)
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
