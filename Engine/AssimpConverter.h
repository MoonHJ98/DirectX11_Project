#pragma once

#include "Includes.h"

class AssimpConverter
{
public:
	AssimpConverter();
	AssimpConverter(const AssimpConverter& Rhs);
	~AssimpConverter();

public:
	void				ReadFile(wstring _File);
	void				ExportMesh(wstring _SavePath);

	void				ReadBoneData(aiNode* _Node, int _Index, int _Parent);
	void				ReadMeshData(aiNode* _Node, int _Bone);
	void				ReadSkinData();

	void				WriteMeshData(wstring _SavePath);

public:
	void				ClipList(vector<wstring>* list);
	void				ExportAnimClip(UINT index, wstring savePath);

private:
	asClip*				ReadClipData(aiAnimation* animation);
	void				ReadKeyframeData(asClip* clip, aiNode* node, vector<asClipNode>& aiNodeInfos);  // 클립이 몇개 있을지 모르기 때문에 ReadClipData에서 읽은 클립을 그대로 넘겨줘서 처리
	void				WriteClipData(asClip* clip, wstring savePath);

private:
	wstring				File;

	Assimp::Importer*	importer = nullptr;
	const aiScene*		scene = nullptr;

	vector<asBone*>		Bones;
	vector<asMesh*>		Meshes;
};

// Assimp 구조
/*
Importer -> Scene (모델 정보가 있음)

Scene -> Materials(색상과 재질)

Scene -> Meshes (실제 렌더링될 데이터)

Scene -> Scene Root (부모, 자식 관계로 된 트렌스폼 정보를 가지고 있음)
         EX) Body
		       ㄴ바퀴 ---> Transformation(부모와 자식간의 연관간계를 유지 하기위해 들어가 있음), Material(Scene에 있는 Materials를 구분 할 번호), Mesh(Scene에 있는 Meshes를 구분 할 번호)
			   ㄴ바퀴
			   ㄴ포탑 
			       ㄴ 포신
*/

/* 애니메이션
메쉬를 본에 연결 시켜 그릴 것이고  애니메이션의 본과 내 Bone클래스를 매칭 해줄 것임.
애니메이션에 본이 매 프레임마다 움직여야 할 Matrix를 가지고 있음
키 프레임 애니메이션이라함
실무에서는 값이 변하는 시점의 키프레임만 저장 하고 그 사이를 보간 하지만. 여기서는 모두 저장할 것임.


*/