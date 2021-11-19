#pragma once

#include "Base.h"
#include "Includes.h"
#include "InputDevice.h"


class GraphicDevice;
class PipeLine;
class GameObject;
class SceneManager;
class ObjectManager;
class LightManager;
class Light;
class Scene;
class Renderer;

class Management : public Base
{
	SINGLETON(Management)

public:
	virtual int  Update(float _timeDelta) override;
	virtual void Render() override;

public:

	// Input Device
	void					UpdateInputDevice();
	UCHAR					GetDIKeyState(UCHAR KeyID);
	UCHAR					GetDIMouseState(InputDevice::MOUSEKEYSTATE _MouseState);
	LONG					GetDIMouseMove(InputDevice::MOUSEMOVESTATE _MouseState);

	// Pipeline
	Matrix*					GetTransform(_D3DTRANSFORMSTATETYPE _Type);

	// Layer
	void					AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object);
	void					AddLayerTest(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object);


	// Scene
	HRESULT					SetScene(shared_ptr<Scene> scene);

	// Light
	HRESULT					AddLight(LIGHTDESC _LightInfo, wstring _LightTag);
	shared_ptr<Light>		FindLight(wstring _LightTag, UINT _Index);

	// Object
	list<shared_ptr<GameObject>>*  FindGameObjectList(int _Index, wstring _LayerKey);
	shared_ptr<GameObject>         FindGameObject(int _SceneIndex, wstring _LayerKey, int _ObjectIndex = 0);


	map<wstring, shared_ptr<GameObject>>& GetObjectsForImgui();



public:
	void					Initialize(HINSTANCE hInst, HWND hWnd, int SceneCnt, int screenWidth, int screenHeight, bool vsync, bool fullscreen, float screenDepth, float screenNear);

private:
	shared_ptr<SceneManager>		SceneMgr = nullptr;
	shared_ptr<ObjectManager>		ObjectMgr = nullptr;
	shared_ptr<PipeLine>			Pipeline = nullptr;
	shared_ptr<LightManager>        LightMgr = nullptr;
	shared_ptr<Renderer>            renderer = nullptr;

};

