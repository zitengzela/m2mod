#include "BoneComparator.h"
#include "Logger.h"
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include "StringHelpers.h"

M2Lib::BoneComparator::DiffResult M2Lib::BoneComparator::Diff(M2 const* oldM2, M2 const * newM2, bool CompareTextures, bool predictScale, float& sourceScale)
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

	std::set<uint16_t> matchedIndices;

	auto maxZSearcher = [](CElement_SubMesh* submeshes, uint32_t submeshCount, uint16_t* Indices, CVertex* vertices) -> float
	{
		float maxZ = 0.0f;
		for (uint32_t i = 0; i < submeshCount; ++i)
		{
			auto& subset = submeshes[i];
			if (subset.ID != 0)
				continue;

			for (uint32_t j = subset.VertexStart; j < subset.VertexStart + subset.VertexCount; ++j)
			{
				auto& vertex = vertices[Indices[j]];

				maxZ = std::max(vertex.Position.Z, maxZ);
			}
		}

		return maxZ;
	};

	if (predictScale)
	{
		auto oldMaxZ = maxZSearcher(OldSubsets, OldSubsetCount, OldIndices, OldVertices);
		auto newMaxZ = maxZSearcher(NewSubsets, NewSubsetCount, NewIndices, NewVertices);

		sourceScale = newMaxZ / oldMaxZ;
	}

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
				auto& _OldVertex = OldVertices[OldIndices[k]];
				auto OldVertex = _OldVertex;
				OldVertex.Position = OldVertex.Position * sourceScale;

				for (uint32_t l = NewSubSet.VertexStart; l < NewSubSet.VertexStart + NewSubSet.VertexCount; ++l)
				{
					auto& NewVertex = NewVertices[NewIndices[l]];

					if (!CVertex::CompareSimilar(OldVertex, NewVertex, CompareTextures, false, false, 1e-5f, 1e-5f))
						continue;

					matchedIndices.insert(OldIndices[k]);

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

						if (candidates.empty())
							OldToNewBoneMap[OldVertex.BoneIndices[i]] = Candidates();
						else for (auto c : candidates)
							OldToNewBoneMap[OldVertex.BoneIndices[i]].AddCandidate(c);
					}
				}
			}
		}
	}

	std::unordered_map<uint32_t, std::unordered_map<uint32_t, float>> res;
	for (auto itr : OldToNewBoneMap)
		res[itr.first] = itr.second.GetWeightedCandidates();

	return { res, matchedIndices.size() * 1.f / OldSkin->Elements[EElement_VertexLookup].Count };
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

M2Lib::BoneComparator::ComparatorWrapper::ComparatorWrapper(M2 const* oldM2, M2 const* newM2, float weightThreshold, bool compareTextures, bool predictScale, float& sourceScale)
{
	auto diff = Diff(oldM2, newM2, compareTextures, predictScale, sourceScale);
	diffMap = diff.map;
	compareStatus = GetDifferenceStatus(diffMap, weightThreshold);

	std::wstringstream ss;

	if (compareStatus == CompareStatus::Identical)
	{
		ss << L"# Old and new model bones are identical" << std::rendl;
		buffer = ss.str();
		return;
	}

	if (compareStatus == CompareStatus::IdenticalWithinThreshold)
	{
		ss << L"# Old and new model bones are identical within given threshold " << std::setprecision(2) << weightThreshold << std::rendl;
		buffer = ss.str();
		return;
	}

	ss << L"# Old M2: " << oldM2->GetFileName() << std::rendl;
	ss << L"# New M2: " << newM2->GetFileName() << std::rendl;
	ss << L"# Weight threshold: " << std::setprecision(2) << weightThreshold << std::rendl;
	ss << L"# Source scale: " << std::setprecision(3) << sourceScale << (predictScale ? " (predicted)" : "") << std::rendl;
	ss << L"# Matched vertices: " << diff.matchedPercent * 100.0f << '%' << std::rendl;
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
			typedef std::pair<uint16_t, float> tty;


			std::vector<tty> sorted(weighted.begin(), weighted.end());

			std::sort(sorted.begin(), sorted.end(), [](tty const& left, tty const& right)
			{
				return right.second < left.second;
			});

			for (auto candidate : sorted)
			{
				ss << std::to_wstring(candidate.first);
				if (sorted.size() > 1)
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

std::unordered_map<uint32_t, float> M2Lib::BoneComparator::Candidates::GetWeightedCandidates()
{
	uint32_t totalCnt = 0;

	for (auto itr : BoneUsage)
		totalCnt += itr.second;

	std::unordered_map<uint32_t, float> ret;
	if (totalCnt == 0)
		return ret;

	for (auto itr : BoneUsage)
		ret[itr.first] = itr.second * 1.0f / totalCnt;

	return ret;
}

M2LIB_HANDLE M2Lib::BoneComparator::Wrapper_Create(M2LIB_HANDLE oldM2, M2LIB_HANDLE newM2, float weightThreshold, bool compareTextures, bool predictScale, float& sourceScale)
{
	try
	{
		return new ComparatorWrapper(static_cast<M2 const*>(oldM2), static_cast<M2 const*>(newM2), weightThreshold, compareTextures, predictScale, sourceScale);
	}
	catch (std::exception & e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return nullptr;
	}
}

M2Lib::BoneComparator::CompareStatus M2Lib::BoneComparator::Wrapper_GetResult(M2LIB_HANDLE pointer)
{
	try
	{
		return static_cast<ComparatorWrapper*>(pointer)->GetResult();
	}
	catch (std::exception & e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return CompareStatus::Identical;
	}
}

const wchar_t* M2Lib::BoneComparator::Wrapper_GetStringResult(M2LIB_HANDLE pointer)
{
	try
	{
		return static_cast<ComparatorWrapper*>(pointer)->GetStringResult();
	}
	catch (std::exception & e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return L"";
	}
}

uint32_t M2Lib::BoneComparator::Wrapper_DiffSize(M2LIB_HANDLE pointer)
{
	try
	{
		return static_cast<ComparatorWrapper*>(pointer)->DiffSize();
	}
	catch (std::exception & e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return 0;
	}
}

void M2Lib::BoneComparator::Wrapper_Free(M2LIB_HANDLE pointer)
{
	try
	{
		delete static_cast<ComparatorWrapper*>(pointer);
	}
	catch (std::exception & e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());
	}
}
