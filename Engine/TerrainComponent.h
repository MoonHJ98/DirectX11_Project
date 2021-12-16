#pragma once

#include "Component.h"

class TerrainComponent : public Component, public enable_shared_from_this<TerrainComponent>
{
private:
	TerrainComponent();
	TerrainComponent(const TerrainComponent& Rhs);

public:
	~TerrainComponent();

private:
	HRESULT Initialize();

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;

	virtual void Render() override;

	virtual void RenderInspector() override;

public:
	bool IsTerrainComponentOpened() { return isTerrainComponentOpened; }

public:
	static shared_ptr<TerrainComponent> Create();

private:
	bool isTerrainComponentOpened = false;



};

