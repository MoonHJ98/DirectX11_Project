#pragma once


class Management;
class GameObject;

class ImguiManager
{
	SINGLETON(ImguiManager);

public:
	HRESULT Initialize();
	void Render();

private:
	void Frame();
	void Scene();
	void Menu1();
	void Hierarchy();
	void Inspector();
	
private:
	void ObjectPopup();
	void ObjectInspector();
	void ComponentPopup();


private:
	shared_ptr<Management> Manage = nullptr;
	bool isObjectInspector = false;

	weak_ptr<GameObject> objectForInspector;
};

