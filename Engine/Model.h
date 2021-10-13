#pragma once

#include "Includes.h"


class AssimpConverter;
class Bone;
class Mesh;
class Shader;

class Model
{
public:
	Model();
	~Model();

public:
	void						  ReadMesh(wstring filePath);
	void						  ReadMaterial();
	shared_ptr<Texture>			  MatchTexture(TEXTUREDESC& _texture);
	void						  BindBone();
	void						  BindMesh();
								  
	void						  UpdateTransform(shared_ptr<Bone> bone = nullptr, const Matrix& matrix = Matrix());
	void						  UpdateBones(shared_ptr<Bone> bone, const Matrix& matrix);

public:
	int							  Update();
	void						  Render();

private:
	shared_ptr<Bone>              Root = nullptr;
	vector<shared_ptr<Bone>>	  bones;
	vector<shared_ptr<Mesh>>	  meshes;
	vector<shared_ptr<Texture>>	  Textures;

	shared_ptr<Shader>			  shader = nullptr;


	Matrix transforms[MAX_MODEL_TRANSFORMS]; // Bone�� ��ü matrix

	AssimpConverter* a;
};

