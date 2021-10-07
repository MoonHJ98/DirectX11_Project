#pragma once

#include "Includes.h"

class Texture;
class AnimationController;

class DynamicModel
{
private:
	DynamicModel() {}
	DynamicModel(const DynamicModel& Rhs) {}

public:
	~DynamicModel();

private:
	HRESULT Initialize(ID3D11Device* _Device, string _MeshFilePath, wstring _VSPath, wstring _PSPath);

public:
	HRESULT				LoadModel(string _MeshFilePath);
	HRESULT				InitializeShader(ID3D11Device * _Device, wstring _VSPath, wstring _PSPath);


public:
	virtual void Render(MATRIXBUFFERTYPE _Matrixbuffer, float _timeDelta);

public:
	static shared_ptr<DynamicModel> Create(ID3D11Device* _Device, string _MeshFilePath, wstring _VSPath, wstring _PSPath);

private:
	vector<shared_ptr<MESH>>			Meshes;
	vector<shared_ptr<Texture>>			Textures;
	shared_ptr<AnimationController>		Animator = nullptr;
	// Shader.
	ComPtr<ID3D11VertexShader>			VertexShader = nullptr;
	ComPtr<ID3D11PixelShader>			PixelShader = nullptr;
	ComPtr<ID3D11InputLayout>			Layout = nullptr;
	ComPtr<ID3D11SamplerState>			SampleState = nullptr;

	// Constant Buffer.
	ConstantBuffer<MATRIXBUFFERTYPE>    MatrixBuffer;
	ConstantBuffer<vector<XMFLOAT4X4>>  BoneTransformBuffer;
};

