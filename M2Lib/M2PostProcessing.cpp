#include "M2.h"
#include "Logger.h"
#include <set>

using namespace M2Lib::M2Element;
using namespace M2Lib::M2Chunk;
using namespace M2Lib::M2SkinElement;

namespace M2Lib
{
	class EdgeTriangleLookup
	{
		M2Skin* skin = nullptr;

		std::map<CElement_SubMesh const*, std::list<std::shared_ptr<Geometry::Triangle>>> edgeTriangleMap;

	public:
		void Initialize(M2Skin* skin)
		{
			this->skin = skin;
		}

		std::list<std::shared_ptr<Geometry::Triangle>> const* GetEdgeTriangles(CElement_SubMesh const* submesh)
		{
			auto itr = edgeTriangleMap.find(submesh);
			if (itr != edgeTriangleMap.end())
				return &itr->second;

			auto res = edgeTriangleMap.emplace(submesh, skin->GetEdgeTriangles(submesh));

			return &res.first->second;
		}
	};
}

M2Lib::EdgeTriangleLookup triangleLookup;

void M2Lib::M2::FixNormals(float AngularTolerance)
{
	auto pSkin = Skins[0];
	auto& SubMeshes = pSkin->Elements[EElement_SubMesh];
	
	auto allMeshes = pSkin->Elements[EElement_SubMesh].as<CElement_SubMesh>();
	uint32_t meshCount = pSkin->Elements[EElement_SubMesh].Count;
	std::list<CElement_SubMesh const*> body, armor, rest;

	uint32_t unkCount = 0;
	for (uint32_t i = 0; i < meshCount; ++i)
	{
		auto submesh = &allMeshes[i];

		if (normalizationRules.IsMatch(submesh->ID))
		{
			body.push_back(submesh);
			continue;
		}

		switch (GetSubSetType(submesh->ID))
		{
			case Subset_Body:
				body.push_back(submesh);
				break;
			case Subset_Armor:
				armor.push_back(submesh);
				break;
			case Subset_Unknown:
				++unkCount;
				rest.push_back(submesh);
				break;
			default:
				rest.push_back(submesh);
				break;
		}
	}

	sLogger.LogInfo(L"Body mesh count: %u, armor mesh count: %u, rest mesh count: %u", body.size(), armor.size(), rest.size());

	//sLogger.LogInfo(L"Body meshes:");
	//for (auto id : bodyMeshIds)
	//	sLogger.LogInfo(L"%u", id);

	triangleLookup.Initialize(pSkin);

	FixNormals(body, body, -1.0f, false);
	FixNormals(body, armor, -1.0f, true);
}

void M2Lib::M2::FixNormals(std::list<CElement_SubMesh const*> const& source, std::list<CElement_SubMesh const*> const& target, float AngularTolerance, bool preferSource)
{
	auto pSkin = Skins[0];
	auto VertexList = Elements[EElement_Vertex].as<CVertex>();
	auto VertexCount = Elements[EElement_Vertex].Count;

	// lookup hash map to make it slow af
	std::set<uint32_t> processedVertices;
	for (auto SubmeshI : source)
	{
		//sLogger.LogInfo("Mesh %u Edges %u", SubmeshI->ID, EdgesI.size());
		std::unordered_set<std::shared_ptr<Geometry::Vertex>> verticesI;

		for (auto SubmeshJ : target)
		{
			//if (SubmeshI == SubmeshJ)
				//continue;

			if (verticesI.empty())
			{
				auto trianglesI = triangleLookup.GetEdgeTriangles(SubmeshI);
				//sLogger.LogError("Mesh %u edge triangles: %u", SubmeshI->ID, trianglesI.size());
				for (auto& triangle : *trianglesI)
				{
					if (triangle->EdgeA->IsUnique)
					{
						verticesI.insert(triangle->EdgeA->A);
						verticesI.insert(triangle->EdgeA->B);
					}

					if (triangle->EdgeB->IsUnique)
					{
						verticesI.insert(triangle->EdgeB->A);
						verticesI.insert(triangle->EdgeB->B);
					}

					if (triangle->EdgeC->IsUnique)
					{
						verticesI.insert(triangle->EdgeC->A);
						verticesI.insert(triangle->EdgeC->B);
					}
				}
			}

			auto trianglesJ = triangleLookup.GetEdgeTriangles(SubmeshJ);
			//sLogger.LogError("Mesh %u edge triangles: %u", SubmeshJ->ID, trianglesJ.size());
			std::unordered_set<std::shared_ptr<Geometry::Vertex>> verticesJ;
			for (auto& triangle : *trianglesJ)
			{
				if (triangle->EdgeA->IsUnique)
				{
					verticesJ.insert(triangle->EdgeA->A);
					verticesJ.insert(triangle->EdgeA->B);
				}

				if (triangle->EdgeB->IsUnique)
				{
					verticesJ.insert(triangle->EdgeB->A);
					verticesJ.insert(triangle->EdgeB->B);
				}

				if (triangle->EdgeC->IsUnique)
				{
					verticesJ.insert(triangle->EdgeC->A);
					verticesJ.insert(triangle->EdgeC->B);
				}
			}

			for (auto iVertex : verticesI)
			{
				m2lib_assert(iVertex->Index < VertexCount);
				auto vertexI = &VertexList[iVertex->Index];
				if (processedVertices.find(iVertex->Index) != processedVertices.end())
					continue;
				processedVertices.insert(iVertex->Index);

				m2lib_assert(iVertex->pEdge->pTriangle->A < VertexCount);
				auto vertexA1 = &VertexList[iVertex->pEdge->pTriangle->A];
				m2lib_assert(iVertex->pEdge->pTriangle->B < VertexCount);
				auto vertexB1 = &VertexList[iVertex->pEdge->pTriangle->B];
				m2lib_assert(iVertex->pEdge->pTriangle->C < VertexCount);
				auto vertexC1 = &VertexList[iVertex->pEdge->pTriangle->C];
				Geometry::Plane a(vertexA1->Position, vertexB1->Position, vertexC1->Position);

				std::list<std::pair<C3Vector, CVertex*>> normalsToAvg;

				for (auto jVertex : verticesJ)
				{
					if (processedVertices.find(jVertex->Index) != processedVertices.end())
						continue;
					processedVertices.insert(jVertex->Index);
					//if (iVertex->Index == jVertex->Index)
					//	continue;

					auto vertexJ = &VertexList[jVertex->Index];

					static float const tolerance = 1e-4f;

					/*if (!floatEq(vertexI->Position.X, vertexJ->Position.X, tolerance) ||
						!floatEq(vertexI->Position.Y, vertexJ->Position.Y, tolerance) ||
						!floatEq(vertexI->Position.Z, vertexJ->Position.Z, tolerance))
						continue;*/

					if (!CVertex::CompareSimilar(*vertexI, *vertexJ, false, false, false, -1.f, AngularTolerance))
						continue;

					auto vertexA2 = &VertexList[jVertex->pEdge->pTriangle->A];
					auto vertexB2 = &VertexList[jVertex->pEdge->pTriangle->B];
					auto vertexC2 = &VertexList[jVertex->pEdge->pTriangle->C];

					Geometry::Plane b(vertexA2->Position, vertexB2->Position, vertexC2->Position);

					if (AngularTolerance > 0.0f && Geometry::CalculateAngle(a, b) > AngularTolerance)
						continue;

					normalsToAvg.emplace_back(b.Normal, vertexJ);
				}

				if (!normalsToAvg.empty())
				{
					C3Vector newNormal = a.Normal;

					if (!preferSource)
					{
						for (auto itr : normalsToAvg)
							newNormal = newNormal + itr.first;

						// if normal is zero, then possibly something is wrong
						// perhaps it's two surfaces on each other
						if (floatEq(newNormal.Length(), 0.0f))
							continue;

						newNormal.Normalize();

						vertexI->Normal = newNormal;
					}

					for (auto itr : normalsToAvg)
						itr.second->Normal = newNormal;
				}
			}
		}
	}
}

void M2Lib::M2::Scale(float Scale)
{
	// vertices
	{
		uint32_t VertexListLength = Elements[EElement_Vertex].Count;
		CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
		for (uint32_t i = 0; i < VertexListLength; i++)
		{
			CVertex& Vertex = VertexList[i];
			Vertex.Position = Vertex.Position * Scale;
		}
	}

	// bones
	{
		auto boneElement = GetBones();
		uint32_t BoneListLength = boneElement->Count;
		CElement_Bone* BoneList = boneElement->as<CElement_Bone>();
		for (uint32_t i = 0; i < BoneListLength; i++)
		{
			CElement_Bone& Bone = BoneList[i];
			Bone.Position = Bone.Position * Scale;
		}
	}

	// attachments
	{
		auto attachmentElement = GetAttachments();
		uint32_t AttachmentListLength = attachmentElement->Count;
		CElement_Attachment* AttachmentList = attachmentElement->as<CElement_Attachment>();
		for (uint32_t i = 0; i < AttachmentListLength; i++)
		{
			CElement_Attachment& Attachment = AttachmentList[i];
			Attachment.Position = Attachment.Position * Scale;
		}
	}

	// events
	{
		uint32_t EventListLength = Elements[EElement_Event].Count;
		CElement_Event* EventList = Elements[EElement_Event].as<CElement_Event>();
		for (uint32_t i = 0; i < EventListLength; i++)
		{
			CElement_Event& Event = EventList[i];
			Event.Position = Event.Position * Scale;
		}
	}

	// lights
	{
		uint32_t LightListLength = Elements[EElement_Light].Count;
		CElement_Light* LightList = Elements[EElement_Light].as<CElement_Light>();
		for (uint32_t i = 0; i < LightListLength; i++)
		{
			CElement_Light& Light = LightList[i];
			Light.Position = Light.Position * Scale;
		}
	}

	// cameras
	{
		uint32_t CameraListLength = Elements[EElement_Camera].Count;
		CElement_Camera* CameraList = Elements[EElement_Camera].as<CElement_Camera>();
		for (uint32_t i = 0; i < CameraListLength; i++)
		{
			CElement_Camera& Camera = CameraList[i];
			Camera.Position = Camera.Position * Scale;
			Camera.Target = Camera.Target * Scale;
		}
	}

	// ribbon emitters
	{
		uint32_t RibbonEmitterListLength = Elements[EElement_RibbonEmitter].Count;
		CElement_RibbonEmitter* RibbonEmitterList = Elements[EElement_RibbonEmitter].as<CElement_RibbonEmitter>();
		for (uint32_t i = 0; i < RibbonEmitterListLength; i++)
		{
			CElement_RibbonEmitter& RibbonEmitter = RibbonEmitterList[i];
			RibbonEmitter.Position = RibbonEmitter.Position * Scale;
		}
	}

	// particle emitters
	{
		uint32_t ParticleEmitterListLength = Elements[EElement_ParticleEmitter].Count;
		CElement_ParticleEmitter* ParticleEmitterList = Elements[EElement_ParticleEmitter].as<CElement_ParticleEmitter>();
		for (uint32_t i = 0; i < ParticleEmitterListLength; i++)
		{
			CElement_ParticleEmitter& ParticleEmitter = ParticleEmitterList[i];
			ParticleEmitter.Position = ParticleEmitter.Position * Scale;
		}
	}
}

void M2Lib::M2::MirrorCamera()
{
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
	uint32_t CameraCount = Elements[EElement_Camera].Count;
	for (uint32_t iCamera = 0; iCamera < CameraCount; iCamera++)
	{
		if (Cameras->Type == 0)
		{
			Cameras->Position.Y = -Cameras->Position.Y;
			Cameras->Target.Y = -Cameras->Target.Y;
			break;
		}
	}
}

void M2Lib::M2::FixSeamsSubMesh(float PositionalTolerance, float AngularTolerance)
{
	// gather up sub meshes
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > SubMeshes;

	M2SkinElement::CElement_SubMesh* Subsets = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	uint32_t SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (uint32_t i = 0; i < SubsetCount; ++i)
	{
		uint16_t ThisID = Subsets[i].ID;
		bool MakeNew = true;
		for (uint32_t j = 0; j < SubMeshes.size(); j++)
		{
			for (uint32_t k = 0; k < SubMeshes[j].size(); k++)
			{
				if (SubMeshes[j][k]->ID == ThisID)
				{
					MakeNew = false;
					SubMeshes[j].push_back(&Subsets[i]);
					break;
				}
			}
			if (!MakeNew)
				break;
		}
		if (MakeNew)
		{
			std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
			NewSubmeshSubsetList.push_back(&Subsets[i]);
			SubMeshes.push_back(NewSubmeshSubsetList);
		}
	}

	// find and merge duplicate vertices
	uint32_t VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (uint32_t iSubMesh1 = 0; iSubMesh1 < SubMeshes.size(); iSubMesh1++)
	{
		for (uint32_t iSubSet1 = 0; iSubSet1 < SubMeshes[iSubMesh1].size(); iSubSet1++)
		{
			M2SkinElement::CElement_SubMesh* pSubSet1 = SubMeshes[iSubMesh1][iSubSet1];

			uint32_t VertexAEnd = pSubSet1->VertexStart + pSubSet1->VertexCount;
			for (uint32_t iVertexA = pSubSet1->VertexStart; iVertexA < VertexAEnd; iVertexA++)
			{
				bool AddedVertexA = false;
				for (uint32_t iSubSet2 = 0; iSubSet2 < SubMeshes[iSubMesh1].size(); iSubSet2++)
				{
					M2SkinElement::CElement_SubMesh* pSubSet2 = SubMeshes[iSubMesh1][iSubSet2];

					uint32_t VertexBEnd = pSubSet2->VertexStart + pSubSet2->VertexCount;
					for (uint32_t iVertexB = pSubSet2->VertexStart; iVertexB < VertexBEnd; iVertexB++)
					{
						if (iVertexA == iVertexB)
							continue;

						if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, true, PositionalTolerance, AngularTolerance))
						{
							if (!AddedVertexA)
							{
								SimilarVertices.push_back(&VertexList[iVertexA]);
								AddedVertexA = true;
							}

							SimilarVertices.push_back(&VertexList[iVertexB]);
						}
					}
				}

				if (SimilarVertices.size() > 1)
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					float invSimilarCount = 1.f / (float)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					uint8_t NewBoneWeights[BONES_PER_VERTEX], NewBoneIndices[BONES_PER_VERTEX];
					for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					SimilarVertices.clear();
				}
			}
		}
	}
}

void M2Lib::M2::FixSeamsBody(float PositionalTolerance, float AngularTolerance)
{
	// sub meshes that are divided up accross multiple bone partitions will have multiple sub mesh entries with the same ID in the M2.
	// we need to gather each body submesh up into a list and average normals of vertices that are similar between other sub meshes.
	// this function is designed to be used on character models, so it may not work on other models.

	// list of submeshes that make up the body of the character
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > CompiledSubMeshList;

	// gather up the body submeshes
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	uint32_t SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (uint32_t i = 0; i < SubsetCount; i++)
	{
		// determine type of subset
		uint16_t ThisID = SubMeshList[i].ID;
		uint16_t Mod = ThisID;
		while (Mod > 10)
		{
			Mod %= 10;
		}
		if ((ThisID == 0) || (ThisID > 10 && Mod == 1) || (ThisID == 702))
		{
			// this subset is part of the character's body
			// add it to the list of submeshes
			bool MakeNew = true;
			for (uint32_t j = 0; j < CompiledSubMeshList.size(); j++)
			{
				for (uint32_t k = 0; k < CompiledSubMeshList[j].size(); k++)
				{
					if (CompiledSubMeshList[j][k]->ID == ThisID)
					{
						MakeNew = false;
						CompiledSubMeshList[j].push_back(&SubMeshList[i]);
						break;
					}
				}
				if (!MakeNew)
				{
					break;
				}
			}
			if (MakeNew)
			{
				std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
				NewSubmeshSubsetList.push_back(&SubMeshList[i]);
				CompiledSubMeshList.push_back(NewSubmeshSubsetList);
			}
		}
	}

	// find and merge duplicate vertices
	uint32_t VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (int32_t iSubMesh1 = 0; iSubMesh1 < (int32_t)CompiledSubMeshList.size() - 1; iSubMesh1++)
	{
		for (int32_t iSubSet1 = 0; iSubSet1 < (int32_t)CompiledSubMeshList[iSubMesh1].size(); iSubSet1++)
		{
			// gather duplicate vertices
			// for each vertex in the subset, compare it against vertices in the other subsets
			// find duplicates and sum their normals
			uint32_t iVertexAEnd = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart + CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexCount;
			for (uint32_t iVertexA = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart; iVertexA < iVertexAEnd; iVertexA++)
			{
				// gather duplicate vertices from other submeshes
				bool AddedVertex1 = false;
				for (int32_t iSubMesh2 = iSubMesh1 + 1; iSubMesh2 < (int32_t)CompiledSubMeshList.size(); iSubMesh2++)
				{
					// check that we don't check against ourselves
					if (iSubMesh2 == iSubMesh1)
					{
						// other submesh is same as this submesh
						continue;
					}
					// go through subsets
					for (int32_t iSubSet2 = 0; iSubSet2 < (int32_t)CompiledSubMeshList[iSubMesh2].size(); iSubSet2++)
					{
						// go through vertices in subset
						uint32_t iVertexBEnd = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart + CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexCount;
						for (uint32_t iVertexB = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart; iVertexB < iVertexBEnd; iVertexB++)
						{
							if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, true, PositionalTolerance, AngularTolerance))
							{
								// found a duplicate
								if (!AddedVertex1)
								{
									SimilarVertices.push_back(&VertexList[iVertexA]);
									AddedVertex1 = true;
								}
								// add the vertex from the other sub mesh to the list of similar vertices
								SimilarVertices.push_back(&VertexList[iVertexB]);
							}
						}
					}
				}

				// average normals of similar vertices
				if (SimilarVertices.size())
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					float invSimilarCount = 1.0f / (float)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					uint8_t NewBoneWeights[BONES_PER_VERTEX], NewBoneIndices[BONES_PER_VERTEX];
					for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					// clear list
					SimilarVertices.clear();
				}
			}
		}
	}
}

void M2Lib::M2::FixSeamsClothing(float PositionalTolerance, float AngularTolerance)
{
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();

	uint32_t SubMeshListLength = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshBodyList;	// gathered body sub meshes
	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshGarbList;	// gathered clothing sub meshes

	for (uint32_t i = 0; i < SubMeshListLength; i++)
	{
		uint16_t ThisID = SubMeshList[i].ID;

		// gather body sub meshes
		uint16_t Mod = ThisID;
		if (Mod > 10)
		{
			Mod %= 10;
		}
		if (ThisID == 0 || (ThisID > 10 && Mod == 1))
		{
			SubMeshBodyList.push_back(&SubMeshList[i]);
		}
		// gather clothing sub meshes
		else if (
			ThisID == 402 ||	// cloth glove
			ThisID == 403 ||	// leather glove
			ThisID == 404 ||	// plate glove
			ThisID == 802 ||	// straight sleeve
			ThisID == 803 ||	// shaped sleeve
			ThisID == 902 ||	// low pant
			ThisID == 903 ||	// hight pant
			ThisID == 502 ||	// cloth boot
			ThisID == 503 ||	// leather boot
			ThisID == 504 ||	// plate boot
			ThisID == 505 ||	// plate boot 2
			ThisID == 1002 ||	// shirt frill short
			ThisID == 1102 ||	// shirt frill long
			ThisID == 1104 ||	// plate leg
			ThisID == 1202 ||	// tabard
			ThisID == 1302		// skirt
			//ThisID == 1802	// plate belt
			)
		{
			SubMeshGarbList.push_back(&SubMeshList[i]);
		}
	}

	// copy vertex properties from main body vertex to duplicate clothing vertices
	for (uint32_t iSubMeshGarb = 0; iSubMeshGarb < SubMeshGarbList.size(); iSubMeshGarb++)
	{
		M2SkinElement::CElement_SubMesh* pSubMeshGarb = SubMeshGarbList[iSubMeshGarb];
		for (uint32_t iSubMeshBody = 0; iSubMeshBody < SubMeshBodyList.size(); iSubMeshBody++)
		{
			M2SkinElement::CElement_SubMesh* pSubMeshBody = SubMeshBodyList[iSubMeshBody];

			for (int32_t iVertexGarb = pSubMeshGarb->VertexStart; iVertexGarb < pSubMeshGarb->VertexStart + pSubMeshGarb->VertexCount; iVertexGarb++)
			{
				for (int32_t iVertexBody = pSubMeshBody->VertexStart; iVertexBody < pSubMeshBody->VertexStart + pSubMeshBody->VertexCount; iVertexBody++)
				{
					if (CVertex::CompareSimilar(VertexList[iVertexGarb], VertexList[iVertexBody], false, false, true, PositionalTolerance, AngularTolerance))
					{
						// copy position, normal, and bone weights, and bone indices from body vertex to other(clothing) vertex
						CVertex* pVertexOther = &VertexList[iVertexGarb];
						CVertex* pVertexBody = &VertexList[iVertexBody];

						pVertexOther->Position = pVertexBody->Position;
						pVertexOther->Normal = pVertexBody->Normal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pVertexOther->BoneWeights[i] = pVertexBody->BoneWeights[i];
							pVertexOther->BoneIndices[i] = pVertexBody->BoneIndices[i];
						}
					}
				}
			}
		}
	}
}
