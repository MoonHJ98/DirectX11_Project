#pragma once
#include "Scene.h"



class Stage : public Scene
{
private:
	Stage();
	Stage(const Stage& Rhs);
	
public:
	~Stage();

private:
	HRESULT		Initialize(ID3D11Device* _Device);

public:

	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	static Stage* Create(ID3D11Device* _Device);

};

