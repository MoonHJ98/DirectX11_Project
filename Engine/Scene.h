#pragma once
#include "Base.h"
class Scene : public Base
{
public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
};

