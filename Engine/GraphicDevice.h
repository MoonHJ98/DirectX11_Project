#pragma once

#include "Includes.h"

class  GraphicDevice
{
	SINGLETON(GraphicDevice)

public:
	bool						Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);

	void						BeginScene(float red, float green, float blue, float alpha);
	void						EndScene();

	ID3D11Device*				GetDevice();
	ID3D11DeviceContext*		GetDeviceContext();

	void						GetProjectionMatrix(XMMATRIX& projectionMatrix);
	void						GetWorldMatrix(XMMATRIX& worldMatrix);
	void						GetOrthoMatrix(XMMATRIX& orthoMatrix);

	void						GetVideoCardInfo(char* cardName, int& memory);

private:
	bool								m_vsync_enabled = false;
	int									m_videoCardMemory = 0;
	char								m_videoCardDescription[128] = { 0, };
	ComPtr<IDXGISwapChain>				m_swapChain = nullptr;
	ComPtr<ID3D11Device>				m_device = nullptr;
	ComPtr<ID3D11DeviceContext>			m_deviceContext = nullptr;
	ComPtr<ID3D11RenderTargetView>		m_renderTargetView = nullptr;
	ComPtr<ID3D11Texture2D>				m_depthStencilBuffer = nullptr;
	ComPtr<ID3D11DepthStencilState>		m_depthStencilState = nullptr;
	ComPtr<ID3D11DepthStencilView>		m_depthStencilView = nullptr;
	ComPtr<ID3D11RasterizerState>		m_rasterState = nullptr;
	//XMMATRIX					m_projectionMatrix = {};
	//XMMATRIX					m_worldMatrix = {};
	Matrix								m_orthoMatrix = {};
};

