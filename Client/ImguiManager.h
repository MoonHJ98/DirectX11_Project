#pragma once


class Management;

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
	void Menu2();
	void Menu3();
	void Menu4();
	
private:
	void ObjectPopup();

private:
	shared_ptr<Management> Manage = nullptr;
};

