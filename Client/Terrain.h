#pragma once

#include "GameObject.h"

class HeightTerrainBuffer;
class TerrainBuffer;

class Transform;
class Shader;

class Material;
class Management;

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
	Vector3 PickTerrain();

public:
	static shared_ptr<Terrain> Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath = L"");

private:
	shared_ptr<HeightTerrainBuffer> heightTerrainBuffer = nullptr;
	shared_ptr<TerrainBuffer> terrainBuffer = nullptr;

	shared_ptr<Transform> transform = nullptr;
	shared_ptr<Shader> shader = nullptr;
	shared_ptr<Material> material = nullptr;

	shared_ptr<Management> Manage = nullptr;

	UINT terrainWidth, terrainHeight;
};

