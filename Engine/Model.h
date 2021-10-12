#pragma once

#include "Includes.h"


class AssimpConverter;
class Bone;
class Mesh;

class Model
{
public:
	Model();
	~Model();

public:
	void ReadMesh(wstring filePath);
	void BindBone();
	void BindMesh();
	
	void UpdateTransform(Bone* bone = nullptr, const Matrix& matrix = Matrix());
	void UpdateBones(Bone* bone, const Matrix& matrix);

public:
	int Update();
	void Render();

private:
	shared_ptr<Bone> Root = nullptr;
	vector<shared_ptr<Bone>> bones;
	vector<shared_ptr<Mesh>> meshes;



	Matrix transforms[MAX_MODEL_TRANSFORMS]; // Bone¿« ¿¸√º matrix

	AssimpConverter* a;
};

