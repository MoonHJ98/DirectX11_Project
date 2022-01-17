#pragma once

#include "GameObject.h"

class Material;

class ParticleSystem : public GameObject, public enable_shared_from_this<ParticleSystem>
{
private:
	struct ParticleType
	{
		Vector3 position;
		Vector3 color = Vector3(0.f, 0.f, 0.f);
		float velocity = 0.f;
		bool active = false;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Vector3 color;
	};

private:
	ParticleSystem();
	ParticleSystem(const ParticleSystem& Rhs);

public:
	~ParticleSystem();

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

private:
	HRESULT InitializeParticleSystem();
	HRESULT InitializeBuffers();

private:
	void EmitParticles(float _timeDelta);
	void UpdateParticles(float _timeDelta);
	void KillParticles();

private:
	void UpdateBuffers();
	void RenderBuffers();

private:
	HRESULT Initialize(MATERIALDESC& _materialDesc);

public:
	static shared_ptr<ParticleSystem> Create(MATERIALDESC& _materialDesc);

private:
	Vector3 particleDeviation = Vector3(0.f, 0.f, 0.f);
	float particleVelocity = 0.f;
	float particleVelocityVariation = 0.f;
	float particleSize = 0;
	float particlePerSecond = 0.f;
	int   maxParticles = 0;

	int   currentParticleCount = 0;
	float accumulatedTime = 0.f;

	shared_ptr<Material> material = nullptr;
	shared_ptr<ParticleType> particleList = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	shared_ptr<VertexType> vertices;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
};

