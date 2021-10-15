#pragma once

#include "Includes.h"


class AssimpConverter;
class Bone;
class Mesh;
class Clip;
class Texture;
class Shader;
class Animator;

class Model : public enable_shared_from_this<Model>
{
private:
	Model();
	Model(const Model& Rhs);

public:
	~Model();

private:
	HRESULT						  Initialize();

public:
	UINT						  GetBoneCount() { return bones.size(); }
	shared_ptr<Bone>			  GetBoneByIndex(UINT index) { return bones[index]; }

public:
	void						  ReadMesh(wstring filePath);
	void						  ReadMaterial();

	shared_ptr<Texture>			  MatchTexture(TEXTUREDESC& _texture);
	void						  BindBone();
	void						  BindMesh();
								  
	void						  UpdateTransform(shared_ptr<Bone> bone = nullptr, const Matrix& matrix = Matrix());
	void						  UpdateBones(shared_ptr<Bone> bone, const Matrix& matrix);

	void						  SetTransformsSRV(ComPtr<ID3D11ShaderResourceView> srv);

public:
	int							  Update(float _timeDelta);
	void						  Render();

public:
	static shared_ptr<Model>      Create();

private:
	shared_ptr<Bone>              Root = nullptr;
	shared_ptr<Animator>          animator = nullptr;

	vector<shared_ptr<Bone>>	  bones;
	vector<shared_ptr<Mesh>>	  meshes;
	vector<shared_ptr<Texture>>	  textures;

	shared_ptr<Shader>			  shader = nullptr;
	Matrix trans;
	float rad = 0;


	Matrix transforms[MAX_MODEL_TRANSFORMS]; // Bone�� ��ü matrix

};

