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
		aiProcess_ConvertToLeftHanded |     // 왼손좌표계로
		aiProcess_Triangulate		  |		// 삼각형으로 바꾸기
		aiProcess_GenUVCoords		  |		// 모델 형식에 맞게 UV다시 계산
		aiProcess_GenNormals		  |     // Normal Vector 다시 계산
		aiProcess_CalcTangentSpace          // 노말맵핑 할 때 사용
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

	bone->transform = bone->transform * parentMatrix; // 본을 자기를 기준으로 부모만큼 움직일 것임.

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

		UINT startVertex = mesh->Vertices.size(); // 메시가 여러개 이므로 정점을 계속 이어서 저장하기위해 이전의 쌓아놓은 크기를 저장.
		for (UINT v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			srcMesh->mBones[0]->mName;
			// 일반적으로 0으로 고정
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
	// 메쉬 기준으로 본하고 매칭시킬 것임.
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
				// index가 정점의 번호와 일치. boneWeights를 Vertices 사이즈 만큼 assign 했기 때문.
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				boneWeights[index].AddWeights(boneIndex, weight);
			}
		}//for(b)


		// 내 정점에 넣어주기.
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
		w->WriteString(bone->Name);  // String이 없다면 구조체 byte단위로 읽고 쓰는 것이 빠름.
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
	// fbx파일에 여러 애니메이션을 한꺼번에 넣어놨을 경우 클립 이름 얻어 올 수 있음.

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
	clip->FrameRate = (float)animation->mTicksPerSecond; // 몇 프레임인지
	clip->FrameCount = (UINT)animation->mDuration + 1;   // 프레임 갯수. 형변환 하기 때문에  +1 해준다.

	vector<asClipNode> aniNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		// Animation에 있는 Bone 정보
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;

		// 모든 프레임을 저장해 줄 것임. 어느 프레임에서 srt가 바뀌지 않으면 값이 없기 때문에 값을 채워 줄 거임.
		// 키 중에 최대 프레임 수를 구하고 SRT값이 없는 것은 이전 값으로 채워줄 거임.
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumScalingKeys);
		keyCount = max(keyCount, aniNode->mNumRotationKeys);


		for (UINT k = 0; k < keyCount; k++)
		{
			asKeyframeData frameData;

			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size();

			// D3DX_16F_EPSILON : 오차범위(2byte float에 대한 오차값)
			// t는 0, 1, 2....이런식으로 진행
			/*
			mTime(프레임)이 예를들어 0.1이였다. 0.1 - 0 = -0.1 절대값이면 0.1
			이 절대값이 D3DX_16F_EPSILON 보다 작으면 0이라고 간주(데이터가 있다.)
			*/
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mPositionKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));

				bFound = true;
			}

			/*
			애니메이션에서는 회전을 쿼터터니온을 사용
			회전 행렬은 vector값으로 저장하면 오차를 발생하기도 하고 연산량이 많음
			쿼터니언은  x,y,z,w만 있음 x, y, z는 회전에 대한 각도. w는 회전값에 대한 비율
			*/
			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiQuatKey key = aniNode->mRotationKeys[k];
				frameData.Time = (float)key.mTime;

				// dx와 assimp의 데이터 순서가 달라서 memcpy안씀
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

		// 내가 가지고 있는 프레임 갯수보다 실제 프레임갯수보다 작다면 마지막값으로 나머지를 채워주기.
		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			asKeyframeData keyFrame = aniNodeInfo.Keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniNodeInfo.Keyframe.push_back(keyFrame);
		}
		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		// 원본 데이터
		aniNodeInfos.push_back(aniNodeInfo);
	}

	// 내 구조체로 옮기기.
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
