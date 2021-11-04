#pragma once

#include "GameObject.h"

class TerrainBuffer;
class Transform;
class Shader;

class Material;

class Terrain :public GameObject, public enable_shared_from_this<Terrain> 
{
private:
	Terrain();
	Terrain(const Terrain& Rhs);

public:
	~Terrain();

private:
	HRESULT Initialize(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath);

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;

public:
	static shared_ptr<Terrain> Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath = L"");

private:
	shared_ptr<TerrainBuffer> terrainBuffer = nullptr;
	shared_ptr<Transform> transform = nullptr;
	shared_ptr<Shader> shader = nullptr;
	shared_ptr<Material> material = nullptr;
};

