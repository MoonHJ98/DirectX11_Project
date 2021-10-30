#pragma once

#include "Includes.h"

class GraphicDevice;
class GameObject;
class DeferredBuffer;
class LightManager;
class Shader;
class RenderTargetManager;

class Renderer
{
	SINGLETON(Renderer)

public:
	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	typedef list<shared_ptr<GameObject>>		RENDERGROUP;


public:
	HRESULT Initialize();

public:
	void Render();


private:
	void RenderPriority();
	void RenderNonAlpha();
	void RenderAlpha();
	void RenderUI();
	void RenderLightAcc();
	void RenderBlend();

public:
	HRESULT	AddRenderGroup(RENDERID ID, shared_ptr<GameObject> _gameObject);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	RENDERGROUP	RenderGroup[RENDER_END];

	shared_ptr<DeferredBuffer> deferredBuffer;
	//shared_ptr<RenderTargetManager> renderTargetMGR;

	shared_ptr<LightManager> LightMgr;

	shared_ptr<Shader> lightShader = nullptr;
};

