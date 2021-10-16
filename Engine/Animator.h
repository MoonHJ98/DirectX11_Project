#pragma once

#include "Includes.h"

class Clip;
class Model;
class GraphicDevice;

class Animator
{
private:
	Animator();
	Animator(const Animator& Rhs);

public:
	~Animator();

public:
	void Update(float _timeDelta);
	void Render();

public:
	shared_ptr<Clip>			      ClipByName(wstring name);

public:
	void						      ReadClip(wstring file);

private:
	void							  CreateTexture();
	void							  CreateClipTransform(UINT index);

private:
	HRESULT						      Initialize(shared_ptr<Model> _model);
								      
public:							      
	static shared_ptr<Animator>       Create(shared_ptr<Model> _model);

private:
	shared_ptr<GraphicDevice>					  Graphic = nullptr;
	weak_ptr<Model>								  model;
	vector<shared_ptr<Clip>>					  clips;

	// 클립 여러개를 다루기 위해 동적할당 할것임.
	ClipTransform*				  clipTransforms = nullptr;

	ComPtr<ID3D11Texture2D>						  texture = nullptr;
	ComPtr<ID3D11ShaderResourceView>			  srv = nullptr;

	KeyframeDesc								  keyframeDesc;
	shared_ptr<ConstantBuffer<KeyframeDesc>>	  keyframeBuffer;
};

/*
texture를 이용해 애니메이션 할 것임
행 : keyframe
열 : bone
면 : clip
*/