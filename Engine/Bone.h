#pragma once

#include "Includes.h"

class Bone
{
private:
	Bone();
public:
	~Bone();

public:
	int GetIndex() { return index; }

	int GetParentIndex() { return parentIndex; }
	shared_ptr<Bone> GetParentBone() { return parent; }
	void  SetParentBone(shared_ptr<Bone> _parent) { parent = _parent; }

	wstring GetName() { return name; }

	Matrix& GetTransform() { return transform; }
	void SetTransform(Matrix& matrix) { transform = matrix; }

	vector<weak_ptr<Bone>>& GetChilds() { return childs; }

private:
	HRESULT Initialize(int _index, wstring _name, int _parentIndex, Matrix _transform);

public:
	static shared_ptr<Bone> Create(int _index, wstring _name, int _parentIndex, Matrix _transform);
private:
	int index;
	wstring name;

	int parentIndex;
	shared_ptr<Bone> parent;

	Matrix transform;
	vector<weak_ptr<Bone>> childs;
};

