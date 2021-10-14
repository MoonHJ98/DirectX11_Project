#include "pch.h"
#include "AssimpConverter.h"
#include "BinaryFile.h"
#include "Path.h"
#include "String.h"

#define D3DX_16F_EPSILON 4.8875809e-4f

AssimpConverter::AssimpConverter()
{
	importer = new Assimp::Importer();
}

AssimpConverter::AssimpConverter(const AssimpConverter & Rhs)
{
}

AssimpConverter::~AssimpConverter()
{
	delete importer;
}

void AssimpConverter::ReadFile(wstring _File)
{
	this->File = L"../Resources/" + _File;

	scene = importer->ReadFile
	(
		ToString(this->File),
		aiProcess_ConvertToLeftHanded |     // �޼���ǥ���
		aiProcess_Triangulate		  |		// �ﰢ������ �ٲٱ�
		aiProcess_GenUVCoords		  |		// �� ���Ŀ� �°� UV�ٽ� ���
		aiProcess_GenNormals		  |     // Normal Vector �ٽ� ���
		aiProcess_CalcTangentSpace          // �븻���� �� �� ���
	);

	assert(scene != nullptr);


}

void AssimpConverter::ExportMesh(wstring _SavePath)
{
	_SavePath = L"../Resources/" + _SavePath + L".mesh";

	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData();
	WriteMeshData(_SavePath);
}

void AssimpConverter::ReadBoneData(aiNode * _Node, int _Index, int _Parent)
{
	asBone* bone = new asBone();
	bone->Index = _Index;
	bone->Parent = _Parent;
	bone->Name = _Node->mName.C_Str();


	Matrix transform(_Node->mTransformation[0]);
	//bone->transform = XMMatrixTranspose(transform);
	bone->transform = transform;

	Matrix parentMatrix;

	if (_Parent < 0)
		parentMatrix = XMMatrixIdentity();
	else
		parentMatrix = Bones[_Parent]->transform;

	bone->transform = bone->transform * parentMatrix; // ���� �ڱ⸦ �������� �θ�ŭ ������ ����.

	Bones.push_back(bone);

	ReadMeshData(_Node, _Index);

	for (UINT i = 0; i < _Node->mNumChildren; ++i)
	{
		ReadBoneData(_Node->mChildren[i], Bones.size(), _Index);
	}
}

void AssimpConverter::ReadMeshData(aiNode * _Node, int _Bone)
{
	if (_Node->mNumMeshes < 1)
		return;


	for (UINT i = 0; i < _Node->mNumMeshes; ++i)
	{
		asMesh* mesh = new asMesh();
		mesh->Name = _Node->mName.C_Str();
		mesh->BoneIndex = _Bone;

		UINT index = _Node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->MaterialName = material->GetName().C_Str();

		UINT startVertex = mesh->Vertices.size(); // �޽ð� ������ �̹Ƿ� ������ ��� �̾ �����ϱ����� ������ �׾Ƴ��� ũ�⸦ ����.
		for (UINT v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			srcMesh->mBones[0]->mName;
			// �Ϲ������� 0���� ����
			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if(srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];
			for (UINT k = 0; k < face.mNumIndices; ++k)
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
		}

		Meshes.push_back(mesh);

	}

}

void AssimpConverter::ReadSkinData()
{
	// �޽� �������� ���ϰ� ��Ī��ų ����.
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false)
			continue;


		asMesh* mesh = Meshes[i];

		vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			UINT boneIndex = 0;
			for (asBone* bone : Bones)
			{
				if (bone->Name == (string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;

					break;
				}
			}//for(bone)

			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				// index�� ������ ��ȣ�� ��ġ. boneWeights�� Vertices ������ ��ŭ assign �߱� ����.
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				boneWeights[index].AddWeights(boneIndex, weight);
			}
		}//for(b)


		// �� ������ �־��ֱ�.
		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[w].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

void AssimpConverter::WriteMeshData(wstring _SavePath)
{
	Path::CreateFolders(Path::GetDirectoryName(_SavePath));
	
	BinaryWriter* w = new BinaryWriter();

	w->Open(_SavePath);

	w->WriteUInt(Bones.size());
	for (asBone* bone : Bones)
	{
		w->WriteInt(bone->Index);
		w->WriteString(bone->Name);  // String�� ���ٸ� ����ü byte������ �а� ���� ���� ����.
		w->WriteInt(bone->Parent);
		w->WriteMatrix(bone->transform);

		SAFEDELETE(bone);
	}

	w->WriteUInt(Meshes.size());
	for (asMesh* meshData : Meshes)
	{
		w->WriteString(meshData->Name);
		w->WriteInt(meshData->BoneIndex);

		w->WriteString(meshData->MaterialName);

		w->WriteUInt(meshData->Vertices.size());
		w->WriteByte(&meshData->Vertices[0], sizeof(ModelVertex) * meshData->Vertices.size());

		w->WriteUInt(meshData->Indices.size());
		w->WriteByte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		SAFEDELETE(meshData);
	}

	w->Close();
	SAFEDELETE(w);
}

void AssimpConverter::ClipList(vector<wstring>* list)
{
	// fbx���Ͽ� ���� �ִϸ��̼��� �Ѳ����� �־���� ��� Ŭ�� �̸� ��� �� �� ����.

	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];

		list->push_back(ToWString(anim->mName.C_Str()));
	}
}

void AssimpConverter::ExportAnimClip(UINT index, wstring savePath)
{
	savePath = L"../Resources/" + savePath + L".clip";

	asClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath);
}

asClip * AssimpConverter::ReadClipData(aiAnimation * animation)
{
	asClip* clip = new asClip();
	clip->Name = animation->mName.C_Str();
	clip->FrameRate = (float)animation->mTicksPerSecond; // �� ����������
	clip->FrameCount = (UINT)animation->mDuration + 1;   // ������ ����. ����ȯ �ϱ� ������  +1 ���ش�.

	vector<asClipNode> aniNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		// Animation�� �ִ� Bone ����
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;

		// ��� �������� ������ �� ����. ��� �����ӿ��� srt�� �ٲ��� ������ ���� ���� ������ ���� ä�� �� ����.
		// Ű �߿� �ִ� ������ ���� ���ϰ� SRT���� ���� ���� ���� ������ ä���� ����.
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumScalingKeys);
		keyCount = max(keyCount, aniNode->mNumRotationKeys);


		for (UINT k = 0; k < keyCount; k++)
		{
			asKeyframeData frameData;

			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size();

			// D3DX_16F_EPSILON : ��������(2byte float�� ���� ������)
			// t�� 0, 1, 2....�̷������� ����
			/*
			mTime(������)�� ������� 0.1�̿���. 0.1 - 0 = -0.1 ���밪�̸� 0.1
			�� ���밪�� D3DX_16F_EPSILON ���� ������ 0�̶�� ����(�����Ͱ� �ִ�.)
			*/
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mPositionKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));

				bFound = true;
			}

			/*
			�ִϸ��̼ǿ����� ȸ���� �����ʹϿ��� ���
			ȸ�� ����� vector������ �����ϸ� ������ �߻��ϱ⵵ �ϰ� ���귮�� ����
			���ʹϾ���  x,y,z,w�� ���� x, y, z�� ȸ���� ���� ����. w�� ȸ������ ���� ����
			*/
			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiQuatKey key = aniNode->mRotationKeys[k];
				frameData.Time = (float)key.mTime;

				// dx�� assimp�� ������ ������ �޶� memcpy�Ⱦ�
				frameData.Rotation.x = key.mValue.x;
				frameData.Rotation.y = key.mValue.y;
				frameData.Rotation.z = key.mValue.z;
				frameData.Rotation.w = key.mValue.w;

				bFound = true;
			}

			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mScalingKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Scale, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));

				bFound = true;
			}

			if (bFound == true)
				aniNodeInfo.Keyframe.push_back(frameData);
		}//for(k)

		// ���� ������ �ִ� ������ �������� ���� �����Ӱ������� �۴ٸ� ������������ �������� ä���ֱ�.
		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			asKeyframeData keyFrame = aniNodeInfo.Keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniNodeInfo.Keyframe.push_back(keyFrame);
		}
		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		// ���� ������
		aniNodeInfos.push_back(aniNodeInfo);
	}

	// �� ����ü�� �ű��.
	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos);


	return clip;
}

void AssimpConverter::ReadKeyframeData(asClip * clip, aiNode * node, vector<asClipNode>& aiNodeInfos)
{
	asKeyframe* keyframe = new asKeyframe();
	keyframe->BoneName = node->mName.C_Str();

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		asClipNode* asClipNode = NULL;

		for (UINT n = 0; n < aiNodeInfos.size(); n++)
		{
			if (aiNodeInfos[n].Name == node->mName)
			{
				asClipNode = &aiNodeInfos[n];

				break;
			}
		}//for(n)


		asKeyframeData frameData;
		if (asClipNode == NULL)
		{
			Matrix transform(node->mTransformation[0]);
			transform = XMMatrixTranspose(transform);

			frameData.Time = (float)i;
			
			XMMatrixDecompose(&XMLoadFloat3(&frameData.Scale), &XMLoadFloat4(&frameData.Rotation), &XMLoadFloat3(&frameData.Translation), transform);
		}
		else
		{
			frameData = asClipNode->Keyframe[i];
		}

		keyframe->Transforms.push_back(frameData);
	}

	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadKeyframeData(clip, node->mChildren[i], aiNodeInfos);
}

void AssimpConverter::WriteClipData(asClip * clip, wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->WriteString(clip->Name);
	w->WriteFloat(clip->Duration);
	w->WriteFloat(clip->FrameRate);
	w->WriteUInt(clip->FrameCount);

	w->WriteUInt(clip->Keyframes.size());
	for (asKeyframe* keyframe : clip->Keyframes)
	{
		w->WriteString(keyframe->BoneName);

		w->WriteUInt(keyframe->Transforms.size());
		w->WriteByte(&keyframe->Transforms[0], sizeof(asKeyframeData) * keyframe->Transforms.size());

		SAFEDELETE(keyframe);
	}

	w->Close();
	SAFEDELETE(w);
}
