#include "BoneComparator.h"
#include "Logger.h"
#include <map>
#include <set>
#include <sstream>

std::map<uint32_t, std::map<uint32_t, float>> M2Lib::BoneComparator::Diff(M2* oldM2, M2* newM2, bool CompareTextures)
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

	auto BoneElement = newM2->GetBones();

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

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::GetDifferenceStatus(std::map<uint32_t, std::map<uint32_t, float>> const& WeightedResult, float weightThreshold)
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
