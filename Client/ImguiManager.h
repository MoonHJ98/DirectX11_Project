#pragma once

class Management;

class ImguiManager
{
	SINGLETON(ImguiManager);

public:
	HRESULT Initialize();
	void Render();

private:
	void DirectionalLight();

private:
	shared_ptr<Management> Manage = nullptr;
};

