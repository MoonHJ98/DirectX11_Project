#pragma once

#include "Includes.h"

class Bone
{
public:
	Bone();
	~Bone();

public:
	int GetIndex() { return index; }

	int ParentIndex() { return parentIndex; }
	Bone* GetParent() { return parent; }

	wstring GetName() { return name; }

	Matrix& GetTransform() { return transform; }
	void SetTransform(Matrix& matrix) { transform = matrix; }

	vector<Bone*>& GetChilds() { return childs; }

private:
	int index;
	wstring name;

	int parentIndex;
	Bone* parent;

	Matrix transform;
	vector<Bone *> childs;
};

