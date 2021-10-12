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
	
private:
	shared_ptr<Bone> Root = nullptr;
	vector<shared_ptr<Bone>> bones;
	vector<shared_ptr<Mesh>> meshes;
	//AssimpConverter* a;
};

