#pragma once

#include "Includes.h"

class GraphicDevice;
class GameObject;
class DeferredBuffer;
class LightManager;
class Shader;
class RenderTargetManager;
class RectangleBuffer;
class Transform;
class Management;

class Renderer
{
	SINGLETON(Renderer)

public:
	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	typedef list<shared_ptr<GameObject>>		RENDERGROUP;

	struct ProjtoWorldType //16바이트의 배수로 패딩
	{
		Matrix ProjInv;
		Matrix ViewInv;
		Vector3 CamPos;
		float padding;
	};

public:
	HRESULT Initialize();
	ID3D11ShaderResourceView* GetBlendTexture();

public:
	void Render();

private:
	void RenderPriority();
	void RenderNonAlpha();
	void RenderAlpha();
	void RenderUI();
	void RenderLight();
	void RenderBlend();

public:
	HRESULT	AddRenderGroup(RENDERID ID, shared_ptr<GameObject> _gameObject);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	RENDERGROUP	RenderGroup[RENDER_END];

	shared_ptr<DeferredBuffer> deferredBuffer;

	shared_ptr<LightManager> LightMgr;

	shared_ptr<Shader> lightShader = nullptr;
	shared_ptr<Shader> blendShader = nullptr;

	shared_ptr<RectangleBuffer> rectangleBuffer = nullptr;

	shared_ptr<ConstantBuffer<ProjtoWorldType>> projToWorld;
	shared_ptr<Management> Manage = nullptr;
};

