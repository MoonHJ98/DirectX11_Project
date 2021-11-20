#pragma once

#include "Component.h"


class AssimpConverter;
class Bone;
class Mesh;
class Clip;
class Texture;
class Animator;
class Material;
class GameObject;

class Light;

class Model : public Component, public enable_shared_from_this<Model>
{
private:
	Model();
	Model(const Model& Rhs);

public:
	~Model();

private:
	HRESULT						  Initialize(shared_ptr<GameObject> _object);

public:
	UINT						  GetBoneCount() { return bones.size(); }
	shared_ptr<Bone>			  GetBoneByIndex(UINT index) { return bones[index]; }

public:
	void						  ReadMesh(wstring filePath);
	void						  ReadMaterial(wstring filePath);
	void						  ReadMaterial();

	shared_ptr<Texture>			  MatchTexture(TEXTUREDESC& _texture);
	void						  BindBone();
	void						  BindMesh();
								  
	void						  UpdateTransform(shared_ptr<Bone> bone = nullptr, const Matrix& matrix = Matrix());
	void						  UpdateBones(shared_ptr<Bone> bone, const Matrix& matrix);

	void						  SetTransformsSRV(ComPtr<ID3D11ShaderResourceView> srv);

public:
	virtual int							  Update(float _timeDelta) override;
	virtual void						  Render() override;
	virtual void RenderInspector() override;

public:
	static shared_ptr<Model>      Create(shared_ptr<GameObject> _object);

private:
	shared_ptr<Bone>              Root = nullptr;
	shared_ptr<Animator>          animator = nullptr;

	vector<shared_ptr<Bone>>	  bones;
	vector<shared_ptr<Mesh>>	  meshes;
	vector<shared_ptr<Texture>>	  textures;
	vector<shared_ptr<Material>>  materials;

	Matrix transforms[MAX_MODEL_TRANSFORMS]; // Bone�� ��ü matrix

	weak_ptr<GameObject> object;

};

