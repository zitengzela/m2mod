#include "M2I.h"
#include "M2Types.h"
#include <assert.h>
#include "DataBinary.h"
#include "M2.h"
#include "Logger.h"
#include <functional>
#include "StringHelpers.h"
#include "FileStorage.h"
#include "Shaders.h"

M2Lib::EError M2Lib::M2I::Load(wchar_t const* FileName, M2Lib::M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool IgnoreOriginalMeshIndexes)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToImportM2I_CouldNotOpenFile;
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// load signature
	uint32_t InSignature = 0;
	InSignature = DataBinary.ReadFourCC();
	if (InSignature != 1 && InSignature != Signature_M2I0)
		return EError_FailedToImportM2I_FileCorrupt;

	// load version
	uint32_t Version = 0;
	if (InSignature == Signature_M2I0)
	{
		uint16_t VersionMajor = DataBinary.Read<uint16_t>();
		uint16_t VersionMinor = DataBinary.Read<uint16_t>();

		Version = MAKE_VERSION(VersionMajor, VersionMinor);

		if (!(Version >= MAKE_VERSION(4, 5) && Version <= MAKE_VERSION(4, 9)) && !(Version >= MAKE_VERSION(8, 0) && Version <= MAKE_VERSION(8, 1)))
			return EError_FailedToImportM2I_UnsupportedVersion;
	}

	// load sub meshes, build new vertex list
	uint32_t VertexStart = 0;
	uint32_t InSubsetCount = 0;
	InSubsetCount = DataBinary.Read<uint32_t>();
	uint32_t iTriangle = 0;

	for (uint32_t i = 0; i < InSubsetCount; i++)
	{
		CSubMesh* pNewSubMesh = new M2I::CSubMesh();

		// read id
		pNewSubMesh->ID = DataBinary.Read<uint16_t>();
		pNewSubMesh->ExtraData.ID = pNewSubMesh->ID;

		if (Version >= MAKE_VERSION(4, 6))
			pNewSubMesh->ExtraData.Description = StringHelpers::trim_copy(DataBinary.ReadASCIIString());
		if (Version >= MAKE_VERSION(4, 7))
		{
			pNewSubMesh->ExtraData.MaterialOverride = DataBinary.Read<int16_t>();
			
			if (Version >= MAKE_VERSION(8, 1))
			{
				pNewSubMesh->ExtraData.ShaderId = DataBinary.Read<int32_t>();

				pNewSubMesh->ExtraData.BlendMode = DataBinary.Read<int16_t>();
				pNewSubMesh->ExtraData.RenderFlags = DataBinary.Read<uint16_t>();

				for (uint32_t j = 0; j < MAX_SUBMESH_TEXTURES; ++j)
				{
					pNewSubMesh->ExtraData.TextureType[j] = DataBinary.Read<int16_t>();
					pNewSubMesh->ExtraData.TextureName[j] = StringHelpers::trim_copy(DataBinary.ReadASCIIString());
				}
			}
			else
			{
				if (Version >= MAKE_VERSION(4, 9))
				{
					if (DataBinary.Read<uint8_t>() != 0)
					{
						pNewSubMesh->ExtraData.ShaderId = TRANSPARENT_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[0] = (int16_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.TextureName[0] = StringHelpers::trim_copy(DataBinary.ReadASCIIString());

						pNewSubMesh->ExtraData.RenderFlags = (int16_t)M2Element::CElement_TextureFlag::EFlags::EFlags_TwoSided;
						pNewSubMesh->ExtraData.BlendMode = DataBinary.Read<uint16_t>();
					}
					else
					{
						DataBinary.ReadASCIIString();
						DataBinary.Read<uint16_t>();
					}

					if (DataBinary.Read<uint8_t>() != 0)
					{
						pNewSubMesh->ExtraData.ShaderId = GLOSS_SHADER_ID;

						pNewSubMesh->ExtraData.TextureType[1] = (int16_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.TextureName[1] = StringHelpers::trim_copy(DataBinary.ReadASCIIString());
					}
					else
						DataBinary.ReadASCIIString();
				}
				else
				{
					pNewSubMesh->ExtraData.TextureName[0] = StringHelpers::trim_copy(DataBinary.ReadASCIIString());
					if (pNewSubMesh->ExtraData.TextureName[0].length() > 0)
					{
						pNewSubMesh->ExtraData.ShaderId = TRANSPARENT_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[0] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.RenderFlags = (int32_t)M2Element::CElement_TextureFlag::EFlags::EFlags_TwoSided;
						pNewSubMesh->ExtraData.BlendMode = (int32_t)M2Element::CElement_TextureFlag::EBlend::EBlend_Decal;
					}

					pNewSubMesh->ExtraData.TextureName[1] = StringHelpers::trim_copy(DataBinary.ReadASCIIString());
					if (pNewSubMesh->ExtraData.TextureName[1].length() > 0)
					{
						pNewSubMesh->ExtraData.ShaderId = GLOSS_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[1] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
					}
				}
			}
		}

		if (Version >= MAKE_VERSION(8, 0))
		{
			if (!IgnoreOriginalMeshIndexes)
				pNewSubMesh->ExtraData.OriginalSubmeshIndex = DataBinary.Read<int32_t>();
			else
				DataBinary.Read<int32_t>();
		}

		// FMN 2015-02-13: read level
		DataBinary.Read<uint16_t>();
		pNewSubMesh->Level = 0; 

		// read vertices
		uint32_t InVertexCount = 0;
		InVertexCount = DataBinary.Read<uint32_t>();
		if (VertexList.size() + InVertexCount > 0xFFFF)
			return EError_FailedToImportM2I_TooManyVertices;

		std::vector<CVertex> submeshVertices;
		for (uint32_t j = 0; j < InVertexCount; ++j)
		{
			CVertex InVertex;

			InVertex.Position = DataBinary.ReadC3Vector();

			for (uint32_t k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneWeights[k] = DataBinary.Read<uint8_t>();

			for (uint32_t k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneIndices[k] = DataBinary.Read<uint8_t>();

			InVertex.Normal = DataBinary.ReadC3Vector();
			InVertex.Texture[0] = DataBinary.ReadC2Vector();
			if (Version >= MAKE_VERSION(8, 0))
				InVertex.Texture[1] = DataBinary.ReadC2Vector();

			uint16_t VertexIndex = (uint16_t)VertexList.size();
			VertexList.push_back(InVertex);
			pNewSubMesh->Indices.push_back(VertexIndex);

			submeshVertices.push_back(InVertex);
		}

		pNewSubMesh->ExtraData.Boundary.Calculate(submeshVertices);

		// read triangles
		uint32_t InTriangleCount = DataBinary.Read<uint32_t>();

		for (uint32_t j = 0; j < InTriangleCount; ++j)
		{
			CTriangle NewTriangle;

			NewTriangle.TriangleIndex = iTriangle;
			++iTriangle;

			for (uint32_t k = 0; k < VERTEX_PER_TRIANGLE; ++k)
				NewTriangle.Vertices[k] = DataBinary.Read<uint16_t>() + VertexStart;

			pNewSubMesh->Triangles.push_back(NewTriangle);
		}

		VertexStart += InVertexCount;

		SubMeshList.push_back(pNewSubMesh);
	}

	auto textureLoop = [=] (std::function<void(CSubMesh*, int)> callback)
	{
		for (auto submesh : SubMeshList)
			for (uint32_t i = 0; i < MAX_SUBMESH_TEXTURES; ++i)
				callback(submesh, i);
	};

	textureLoop([](CSubMesh* submesh, int textureIndex)
	{
		if (submesh->ExtraData.TextureType[textureIndex] == (int16_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded && submesh->ExtraData.TextureName[textureIndex].size() <= 1)
		{
			if (textureIndex == 0 || submesh->ExtraData.ShaderId != -1 && GetOpCountForShader(submesh->ExtraData.ShaderId) > textureIndex)
				sLogger.LogError(L"Submesh with id [%u] has 'hardcoded' texture type, but texture path is not set", submesh->ID);
		}
	});

	textureLoop([=](CSubMesh* submesh, int textureIndex)
	{
		auto const& path = submesh->ExtraData.TextureName[textureIndex];
		if (submesh->ExtraData.TextureType[textureIndex] == (int16_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded && path.length() > 0) {
			sLogger.LogInfo(L"Used custom texture with path: '%s'", StringHelpers::StringToWString(path).c_str());
		}
	});

	std::map<uint16_t, uint16_t> BoneRemap;

	if (!IgnoreBones)
	{
		// read bones, overwrite existing
		auto boneElement = pM2->GetBones();
		uint32_t BoneCount = boneElement->Count;
		uint32_t BoneCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < BoneCountIn; ++i)
		{
			uint16_t InBoneIndex = DataBinary.Read<uint16_t>();
			int16_t ParentBone = DataBinary.Read<int16_t>();
			C3Vector Position = DataBinary.ReadC3Vector();
			bool HasExtraData = false;
			uint32_t Flags;
			uint16_t SubmeshId;
			uint16_t Unknown[2];

			if (Version >= MAKE_VERSION(4, 8))
			{
				HasExtraData = DataBinary.Read<uint8_t>() != 0;
				Flags = DataBinary.Read<uint32_t>();
				SubmeshId = DataBinary.Read<uint16_t>();
				Unknown[0] = DataBinary.Read<uint16_t>();
				Unknown[1] = DataBinary.Read<uint16_t>();
			}

			uint16_t ModBoneIndex = -1;
			if (InBoneIndex < BoneCount)
			{
				ModBoneIndex = InBoneIndex;
			}
			else
			{
				sLogger.LogInfo(L"Extra bones detected: skipping");
				continue;
			}
			/*else if (HasExtraData)
			{
				M2Element::CElement_Bone newBone;
				newBone.BoneLookupID = -1;
				ModBoneIndex = pM2->AddBone(newBone);
				BoneRemap[InBoneIndex] = ModBoneIndex;
			}*/

			if (ModBoneIndex == -1)
				continue;

			auto Bones = boneElement->as<M2Element::CElement_Bone>();
			auto& BoneToMod = Bones[ModBoneIndex];

			BoneToMod.ParentBone = ParentBone;
			BoneToMod.Position = Position;

			if (HasExtraData)
			{
				BoneToMod.Flags = (M2Element::CElement_Bone::EFlags)Flags;
				BoneToMod.SubmeshId = SubmeshId;
				BoneToMod.Unknown[0] = Unknown[0];
				BoneToMod.Unknown[1] = Unknown[1];
			}
		}
	}
	else
	{
		uint32_t BoneCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < BoneCountIn; ++i)
		{
			DataBinary.Read<uint16_t>();
			DataBinary.Read<int16_t>();
			DataBinary.ReadC3Vector();

			if (Version >= MAKE_VERSION(4, 8))
			{
				DataBinary.Read<uint8_t>();
				DataBinary.Read<uint32_t>();
				DataBinary.Read<uint16_t>();
				DataBinary.Read<uint16_t>();
				DataBinary.Read<uint16_t>();
			}
		}
	}

	// this is only executed when new bones are present, skip
	/*if (!BoneRemap.empty())
	{
		uint32_t BoneCount = pM2->Elements[M2Element::EElement_Bone].Count;
		auto Bones = pM2->Elements[M2Element::EElement_Bone].as<M2Element::CElement_Bone>();
		for (uint32_t i = 0; i < BoneCount; ++i)
		{
			auto& Bone = Bones[i];
			if (BoneRemap.find(Bone.ParentBone) == BoneRemap.end())
				continue;

			Bone.ParentBone = BoneRemap[Bone.ParentBone];
		}

		for (auto& Vertex : VertexList)
		{
			for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
			{
				if (BoneRemap.find(Vertex.BoneIndices[i]) == BoneRemap.end())
					continue;

				Vertex.BoneIndices[i] = BoneRemap[Vertex.BoneIndices[i]];
			}
		}
	}*/

	if (!IgnoreAttachments)
	{
		// read attachments, overwrite existing
		auto attachmentElement = pM2->GetAttachments();
		uint32_t AttachmentsCount = attachmentElement->Count;
		auto Attachments = attachmentElement->as<M2Element::CElement_Attachment>();
		uint32_t AttachmentCountIn;
		AttachmentCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < AttachmentCountIn; ++i)
		{
			uint32_t InAttachmentID = 0;
			InAttachmentID = DataBinary.Read<uint32_t>();
			M2Element::CElement_Attachment* pAttachmentToMod = NULL;
			for (uint32_t j = 0; j < AttachmentsCount; ++j)
			{
				if (Attachments[j].ID == InAttachmentID)
				{
					pAttachmentToMod = &Attachments[j];
					break;
				}
			}
			if (pAttachmentToMod)
			{
				pAttachmentToMod->ParentBone = DataBinary.Read<int16_t>();
				if (BoneRemap.find(pAttachmentToMod->ParentBone) != BoneRemap.end())
					pAttachmentToMod->ParentBone = BoneRemap[pAttachmentToMod->ParentBone];

				pAttachmentToMod->Position = DataBinary.ReadC3Vector();
				float Scale = DataBinary.Read<float>();
			}
			else
			{
				DataBinary.Read<uint16_t>();

				DataBinary.ReadC3Vector();
				DataBinary.Read<float>();
			}
		}
	}
	else
	{
		uint32_t AttachmentCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < AttachmentCountIn; ++i)
		{
			DataBinary.Read<uint32_t>();
			DataBinary.Read<uint16_t>();
			DataBinary.ReadC3Vector();
			DataBinary.Read<float>();
		}
	}

	if (!IgnoreCameras)
	{
		// read cameras, overwrite existing
		uint32_t CameraCount = pM2->Elements[M2Element::EElement_Camera].Count;
		auto Cameras = pM2->Elements[M2Element::EElement_Camera].as<M2Element::CElement_Camera>();
		uint32_t CameraCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < CameraCountIn; ++i)
		{
			auto hasData = true;
			if (Version >= MAKE_VERSION(4, 9))
				hasData = DataBinary.Read<uint8_t>() != 0;

			M2Element::CElement_Camera* pCameraToMod = NULL;
			if (hasData)
			{
				auto InType = (M2Element::CElement_Camera::ECameraType)DataBinary.Read<int32_t>();
				for (uint32_t j = 0; j < CameraCount; ++j)
				{
					if (Cameras[j].Type == InType)
					{
						pCameraToMod = &Cameras[j];
						break;
					}
				}
			}
			if (pCameraToMod)
			{
				if (pCameraToMod->AnimationBlock_FieldOfView.Values.Count > 0)
				{
					auto ExternalAnimations = (M2Array*)pM2->Elements[M2Element::EElement_Camera].GetLocalPointer(pCameraToMod->AnimationBlock_FieldOfView.Values.Offset);

					auto LastElementIndex = pM2->GetLastElementIndex();
					m2lib_assert(LastElementIndex != M2Element::EElement__Count__);
					auto& LastElement = pM2->Elements[LastElementIndex];
					m2lib_assert(ExternalAnimations[0].Offset >= LastElement.Offset && ExternalAnimations[0].Offset < LastElement.Offset + LastElement.Data.size());

					float* FieldOfView_Keys = (float*)LastElement.GetLocalPointer(ExternalAnimations[0].Offset);
					auto value = DataBinary.Read<float>();
					FieldOfView_Keys[0] = value;
				}
				else
					DataBinary.Read<float>();

				pCameraToMod->ClipFar = DataBinary.Read<float>();
				pCameraToMod->ClipNear = DataBinary.Read<float>();
				pCameraToMod->Position = DataBinary.ReadC3Vector();
				pCameraToMod->Target = DataBinary.ReadC3Vector();
			}
			else
			{
				DataBinary.Read<float>();
				DataBinary.Read<float>();
				DataBinary.Read<float>();
				DataBinary.ReadC3Vector();
				DataBinary.ReadC3Vector();
			}
		}
	}
	else
	{
		uint32_t CameraCountIn = DataBinary.Read<uint32_t>();
		for (uint32_t i = 0; i < CameraCountIn; ++i)
		{
			if (Version >= MAKE_VERSION(4, 9))
				DataBinary.Read<uint8_t>();
			DataBinary.Read<int32_t>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
			DataBinary.Read<float>();
		}
	}

	return EError_OK;
}
