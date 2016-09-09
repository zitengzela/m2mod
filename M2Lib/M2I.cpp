#include "M2I.h"
#include "M2Types.h"
#include <assert.h>
#include "DataBinary.h"
#include "M2.h"

M2Lib::EError M2Lib::M2I::Load(Char16* FileName, M2Lib::M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToImportM2I_CouldNotOpenFile;
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// load signature
	UInt32 InSignature = 0;
	InSignature = DataBinary.ReadFourCC();
	if (InSignature != 1 && InSignature != Signature_M2I0)
		return EError_FailedToImportM2I_FileCorrupt;

	// load version
	UInt16 VersionMajor;
	UInt16 VersionMinor;
	if (InSignature == Signature_M2I0)
	{
		VersionMajor = DataBinary.ReadUInt16();
		VersionMinor = DataBinary.ReadUInt16();
		if (VersionMajor != 4 || VersionMinor != 5 && VersionMinor != 6)
			return EError_FailedToImportM2I_UnsupportedVersion;
	}

	// load sub meshes, build new vertex list
	UInt32 VertexStart = 0;
	UInt32 InSubsetCount = 0;
	InSubsetCount = DataBinary.ReadUInt32();
	UInt32 iTriangle = 0;

	for (UInt32 i = 0; i < InSubsetCount; i++)
	{
		M2I::CSubMesh* pNewSubMesh = new M2I::CSubMesh();

		// read id
		pNewSubMesh->ID = DataBinary.ReadUInt16();
		pNewSubMesh->ComparisonData.ID = pNewSubMesh->ID;
		pNewSubMesh->ComparisonData.M2IIndex = i;

		if (VersionMinor == 6)
			pNewSubMesh->ComparisonData.Description = DataBinary.ReadASCIIString();

		// FMN 2015-02-13: read level
		pNewSubMesh->Level = DataBinary.ReadUInt16();

		// read vertices
		UInt32 InVertexCount = 0;
		InVertexCount = DataBinary.ReadUInt32();
		if (VertexList.size() + InVertexCount > 0xFFFF)
		{
			return EError_FailedToImportM2I_TooManyVertices;
		}

		std::vector<CVertex> submeshVertices;
		for (UInt32 j = 0; j < InVertexCount; j++)
		{
			CVertex InVertex;

			InVertex.Position[0] = DataBinary.ReadFloat32();
			InVertex.Position[1] = DataBinary.ReadFloat32();
			InVertex.Position[2] = DataBinary.ReadFloat32();

			InVertex.BoneWeights[0] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[1] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[2] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[3] = DataBinary.ReadUInt8();

			int WeightSum = InVertex.BoneWeights[0] + InVertex.BoneWeights[1] + InVertex.BoneWeights[2] + InVertex.BoneWeights[3];
			if (WeightSum < 255)
			{
				int pause = 1;
			}

			InVertex.BoneIndices[0] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[1] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[2] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[3] = DataBinary.ReadUInt8();

			InVertex.Normal[0] = DataBinary.ReadFloat32();
			InVertex.Normal[1] = DataBinary.ReadFloat32();
			InVertex.Normal[2] = DataBinary.ReadFloat32();

			InVertex.Texture[0] = DataBinary.ReadFloat32();
			InVertex.Texture[1] = DataBinary.ReadFloat32();

			UInt16 VertexTriangleIndex = VertexList.size();
			VertexList.push_back(InVertex);
			pNewSubMesh->Indices.push_back(VertexTriangleIndex);

			submeshVertices.push_back(InVertex);
		}

		pNewSubMesh->ComparisonData.Boundary.Calculate(submeshVertices);

		// read triangles
		UInt32 InTriangleCount = 0;
		InTriangleCount = DataBinary.ReadUInt32();

		for (UInt32 j = 0; j < InTriangleCount; j++)
		{
			CTriangle NewTriangle;

			NewTriangle.TriangleIndex = iTriangle;
			iTriangle++;

			NewTriangle.Vertices[0] = DataBinary.ReadUInt16() + VertexStart;
			NewTriangle.Vertices[1] = DataBinary.ReadUInt16() + VertexStart;
			NewTriangle.Vertices[2] = DataBinary.ReadUInt16() + VertexStart;

			pNewSubMesh->Triangles.push_back(NewTriangle);
		}

		VertexStart += InVertexCount;

		SubMeshList.push_back(pNewSubMesh);
	}

	if (!IgnoreBones)
	{
		// read bones, overwrite existing
		UInt32 BoneCount = pM2->Elements[M2Element::EElement_Bone].Count;
		auto Bones = pM2->Elements[M2Element::EElement_Bone].as<M2Element::CElement_Bone>();
		UInt32 BoneCountIn;
		BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; i++)
		{
			UInt16 InBoneIndex = 0;
			InBoneIndex = DataBinary.ReadUInt16();
			assert(InBoneIndex < BoneCount);
			auto& BoneToMod = Bones[InBoneIndex];

			BoneToMod.ParentBone = DataBinary.ReadSInt16();

			BoneToMod.Position[0] = DataBinary.ReadFloat32();
			BoneToMod.Position[1] = DataBinary.ReadFloat32();
			BoneToMod.Position[2] = DataBinary.ReadFloat32();
		}
	}
	else
	{
		UInt32 BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; ++i)
		{
			DataBinary.ReadUInt16();
			DataBinary.ReadSInt16();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	if (!IgnoreAttachments)
	{
		// read attachments, overwrite existing
		UInt32 AttachmentsCount = pM2->Elements[M2Element::EElement_Attachment].Count;
		auto Attachments = pM2->Elements[M2Element::EElement_Attachment].as<M2Element::CElement_Attachment>();
		UInt32 AttachmentCountIn;
		AttachmentCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < AttachmentCountIn; i++)
		{
			UInt32 InAttachmentID = 0;
			InAttachmentID = DataBinary.ReadUInt32();
			M2Element::CElement_Attachment* pAttachmentToMod = NULL;
			for (UInt32 j = 0; j < AttachmentsCount; j++)
			{
				if (Attachments[j].ID == InAttachmentID)
				{
					pAttachmentToMod = &Attachments[j];
					break;
				}
			}
			if (pAttachmentToMod)
			{
				pAttachmentToMod->ParentBone = DataBinary.ReadSInt16();

				pAttachmentToMod->Position[0] = DataBinary.ReadFloat32();
				pAttachmentToMod->Position[1] = DataBinary.ReadFloat32();
				pAttachmentToMod->Position[2] = DataBinary.ReadFloat32();
				Float32 Scale = DataBinary.ReadFloat32();
			}
			else
			{
				DataBinary.ReadUInt16();

				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
			}
		}
	}
	else
	{
		UInt32 AttachmentCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < AttachmentCountIn; ++i)
		{
			DataBinary.ReadUInt32();
			DataBinary.ReadUInt16();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	if (!IgnoreCameras)
	{
		// read cameras, overwrite existing
		UInt32 CameraCount = pM2->Elements[M2Element::EElement_Camera].Count;
		auto Cameras = pM2->Elements[M2Element::EElement_Camera].as<M2Element::CElement_Camera>();
		UInt32 CameraCountIn;
		CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; i++)
		{
			SInt32 InType;
			InType = DataBinary.ReadSInt32();

			M2Element::CElement_Camera* pCameraToMod = NULL;
			for (UInt32 j = 0; j < CameraCount; j++)
			{
				if (Cameras[j].Type == InType)
				{
					pCameraToMod = &Cameras[j];
					break;
				}
			}
			if (pCameraToMod)
			{
				pCameraToMod->Type = InType;

				if (pCameraToMod->AnimationBlock_FieldOfView.nKeys > 0)
				{
					auto ExternalAnimations = (M2::SExternalAnimation*)pM2->Elements[M2Element::EElement_Camera].GetLocalPointer(pCameraToMod->AnimationBlock_FieldOfView.oKeys);
					Float32* FieldOfView_Keys = (Float32*)(&pM2->RawData[ExternalAnimations[0].Offset]);
					Float32 InFoV;
					InFoV = DataBinary.ReadFloat32();	// we are writing to RawData in memory, this change will not be saved to the M2 because it lies out of bounds of any elements.
					FieldOfView_Keys[0] = InFoV;
				}
				else
				{
					DataBinary.ReadFloat32();
				}
				pCameraToMod->ClipFar = DataBinary.ReadFloat32();
				pCameraToMod->ClipNear = DataBinary.ReadFloat32();
				pCameraToMod->Position[0] = DataBinary.ReadFloat32();
				pCameraToMod->Position[1] = DataBinary.ReadFloat32();
				pCameraToMod->Position[2] = DataBinary.ReadFloat32();
				pCameraToMod->Target[0] = DataBinary.ReadFloat32();
				pCameraToMod->Target[1] = DataBinary.ReadFloat32();
				pCameraToMod->Target[2] = DataBinary.ReadFloat32();
			}
			else
			{
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
			}
		}
	}
	else
	{
		UInt32 CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; ++i)
		{
			DataBinary.ReadSInt32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	return EError_OK;
}
