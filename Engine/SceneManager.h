#pragma once
#include "Base.h"

class SceneManager : public Base
{
	SINGLETON(SceneManager)

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
};

