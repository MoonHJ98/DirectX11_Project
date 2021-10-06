#pragma once

#include "Includes.h"


class PipeLine
{
	SINGLETON(PipeLine)

public:
	HRESULT			Initialize();
	XMMATRIX*		GetTransform(_D3DTRANSFORMSTATETYPE _Type);
	HRESULT			UpdateViewMatrix(XMVECTOR _PositionVector, XMVECTOR _LookAtVector, XMVECTOR _UpVector);
	HRESULT			UpdateProjectionMatrix(float _FovY, float _Aspect, float _Near, float _Far);

	XMMATRIX		ViewMatrix;
	XMMATRIX		ProjectionMatrix;

};

