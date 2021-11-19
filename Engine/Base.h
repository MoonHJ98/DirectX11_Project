#pragma once

#include "Includes.h"

class Base abstract
{
public:
	Base();
	Base(const Base& Rhs);
	~Base();

protected:
	virtual int Update(float _timeDelta = 0) = 0;
	virtual void Render() = 0;
};

