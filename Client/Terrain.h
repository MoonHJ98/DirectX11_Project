#pragma once

#include "GameObject.h"

class TerrainBuffer;
class Transform;
class Shader;
class Material;
class Management;
class Quadtree;
class PhysXManager;

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
	virtual void OnContact() override;
	virtual void OnTrigger() override;


public:
	void SetScreenPos(Vector2 pos) { screenPos = pos; }
	Vector3 PickTerrain(Vector2 screenPos, Vector2 _screenSize);
	void SetScreenSize(Vector2 _screenSize);

private:
	void RaiseHeight();
	void PaintTexture();
	

public:
	static shared_ptr<Terrain> Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath = L"");

private:
	shared_ptr<TerrainBuffer> terrainBuffer = nullptr;

	shared_ptr<Quadtree> quadtree = nullptr;

	shared_ptr<Shader> shader = nullptr;
	shared_ptr<Material> material = nullptr;

	shared_ptr<Management> Manage = nullptr;

	UINT terrainWidth, terrainHeight;

	Vector2 screenPos = Vector2();
	shared_ptr<PhysXManager> physxManager = nullptr;

};

