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

	// Ŭ�� �������� �ٷ�� ���� �����Ҵ� �Ұ���.
	ClipTransform*				  clipTransforms = nullptr;

	ComPtr<ID3D11Texture2D>						  texture = nullptr;
	ComPtr<ID3D11ShaderResourceView>			  srv = nullptr;

	KeyframeDesc								  keyframeDesc;
	shared_ptr<ConstantBuffer<KeyframeDesc>>	  keyframeBuffer;
};

/*
texture�� �̿��� �ִϸ��̼� �� ����
�� : keyframe
�� : bone
�� : clip
*/