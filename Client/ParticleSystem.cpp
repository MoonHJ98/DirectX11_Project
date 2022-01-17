#include "pch.h"
#include "ParticleSystem.h"
#include "GraphicDevice.h"
#include "Material.h"
#include "Transform.h"
#include "Shader.h"
#include "Renderer.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::ParticleSystem(const ParticleSystem & Rhs)
{
}

ParticleSystem::~ParticleSystem()
{
}

int ParticleSystem::Update(float _TimeDelta)
{
	ComputeViewZ(&transform->GetState(Transform::POSITION));



	UpdateComponent(_TimeDelta);

	// ������ ��ƼŬ�� �����մϴ�.
	KillParticles();

	// �� ��ƼŬ�� �����մϴ�.
	EmitParticles(_TimeDelta);

	// ��ƼŬ ��ġ�� ������Ʈ �մϴ�.
	UpdateParticles(_TimeDelta);

	// ���� ���� ���۸� �� ��ƼŬ�� �� ��ġ�� ������Ʈ�մϴ�.
	UpdateBuffers();

	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_ALPHA, shared_from_this());

	return 0;
}

void ParticleSystem::Render()
{
	//Graphic->TurnOnAlphaBlending();


	RenderComponent();
	RenderBuffers();

	//Graphic->TurnOffAlphaBlending();

}

void ParticleSystem::OnContact()
{
}

void ParticleSystem::OnTrigger()
{
}

HRESULT ParticleSystem::InitializeParticleSystem()
{
	// ���� �� �� ��ƼŬ�� ��ġ �� �� �ִ� ������ ������ ����.
	particleDeviation = Vector3(0.5f, 0.1f, 2.f);

	// ��ƼŬ�� �ӵ��� �ӵ� ��ȭ�� ����.
	particleVelocity = 1.f;
	particleVelocityVariation = 0.2f;

	// ��ƼŬ�� ������ ũ�� ����
	particleSize = 10.f;

	// �ʴ� ���� �� ��ƼŬ �� ����.
	particlePerSecond = 250.f;

	// ��ƼŬ �ý��ۿ� ���Ǵ� ǣ�� ��ƼŬ ���� ����
	maxParticles = 5000;

	// ��ƼŬ ����Ʈ ����.
	shared_ptr<ParticleType> tempParticleList(new ParticleType[maxParticles]);
	particleList = tempParticleList;
	if (particleList == nullptr)
	{
		MSG_BOX("Failed to create Particle List.");
		return E_FAIL;
	}

	// ���� ������� ���� ���� ��ƼŬ ���� 0���� �ʱ�ȭ.
	currentParticleCount = 0;

	// �ʴ� ��ƼŬ ���� �ӵ��� �ʱ� �������� 0���� �ʱ�ȭ.
	accumulatedTime = 0.f;

	return S_OK;
}

HRESULT ParticleSystem::InitializeBuffers()
{
	vertexCount = maxParticles * 6;

	indexCount = vertexCount;

	// ������ �� ��ƼŬ�� ���� ���� �迭 ����.
	shared_ptr<VertexType> tempVertices(new VertexType[vertexCount]);
	vertices = tempVertices;
	if (vertices == nullptr)
	{
		MSG_BOX("Failed to initialize ParticleSystem's vertex buffer.");
		return E_FAIL;
	}
	memset(vertices.get(), 0, sizeof(VertexType) * vertexCount);

	shared_ptr<uint16_t> indices (new uint16_t[indexCount]);
	for (int i = 0; i < indexCount; ++i)
	{
		indices.get()[i] = i;
	}

	// dynamic vertex buffer
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices.get();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	Graphic->GetDevice()->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), indices.get(), indexCount, sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
	
	return S_OK;
}

void ParticleSystem::EmitParticles(float _timeDelta)
{
	// ������ �ð��� ������ŵ�ϴ�.
	accumulatedTime += _timeDelta;

	// ���� ��ƼŬ ����� false�� �����մϴ�.
	bool emitParticle = false;

	// �� ��ƼŬ�� ������ �ð����� Ȯ���մϴ�.
	if (accumulatedTime > (1000.0f / particlePerSecond))
	{
		accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// ���� �� ��ƼŬ�� ������ ������ �� �ϳ��� �����մϴ�.
	if ((emitParticle == true) && (currentParticleCount < (maxParticles - 1)))
	{
		currentParticleCount++;

		// ���� ���� ȭ �� ��ƼŬ �Ӽ��� �����մϴ�.
		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.x;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.y;
		float positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.z;

		Vector3 _position = Vector3(positionX, positionY, positionZ);

		float velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;

		float red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		Vector3 _color = Vector3(red, green, blue);

		// ���� ������ ���� ��ƼŬ�� �ڿ��� ������ �������ؾ��ϹǷ� ��ƼŬ �迭�� �����ؾ� �մϴ�.
		// Z ���̸� ����Ͽ� �����ϹǷ� ��Ͽ��� ��ƼŬ�� �����ؾ� �ϴ� ��ġ�� ã�ƾ��մϴ�.
		int index = 0;
		bool found = false;
		while (!found)
		{
			if ((particleList.get()[index].active == false) || (particleList.get()[index].position.z < _position.z))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// ���� �� ��ġ�� �˾����Ƿ� �ε������� �� ��ġ �� �迭�� �����Ͽ� �� ��ƼŬ�� ���� ������ �������մϴ�.
		int i = currentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			particleList.get()[i].position = particleList.get()[j].position;
			particleList.get()[i].color = particleList.get()[j].color;
			particleList.get()[i].velocity = particleList.get()[j].velocity;
			particleList.get()[i].active = particleList.get()[j].active;
			i--;
			j--;
		}

		// ���� ��Ȯ�� ���� ������ ��ƼŬ �迭�� �����Ͻʽÿ�.
		particleList.get()[index].position = _position;
		particleList.get()[index].color = _color;
		particleList.get()[index].velocity = velocity;
		particleList.get()[index].active = true;
	}
}

void ParticleSystem::UpdateParticles(float _timeDelta)
{
	// �� �������� ��ġ, �ӵ� �� ������ �ð��� ����Ͽ� �Ʒ������� �̵��Ͽ� ��� ��ƼŬ�� ������Ʈ�մϴ�.
	for (int i = 0; i < currentParticleCount; i++)
	{
		particleList.get()[i].position.y = particleList.get()[i].position.y - (particleList.get()[i].velocity * _timeDelta);
	}
}

void ParticleSystem::KillParticles()
{
	// Ư�� ���� ������ ��� ��� ��ƼŬ�� �����մϴ�.
	for (int i = 0; i < maxParticles; i++)
	{
		if ((particleList.get()[i].active == true) && (particleList.get()[i].position.y < -3.0f))
		{
			particleList.get()[i].active = false;
			currentParticleCount--;

			// ���� ��� ����ִ� ��ƼŬ�� �迭���� �̵����� �ı��� ��ƼŬ�� ����� �迭�� �ùٸ��� ���ĵ� ���·� �����մϴ�.
			for (int j = i; j < maxParticles - 1; j++)
			{
				particleList.get()[j].position = particleList.get()[j + 1].position;
				particleList.get()[j].color = particleList.get()[j + 1].color;
				particleList.get()[j].velocity = particleList.get()[j + 1].velocity;
				particleList.get()[j].active = particleList.get()[j + 1].active;
			}
		}
	}
}

void ParticleSystem::UpdateBuffers()
{
	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices.get(), 0, sizeof(VertexType) * vertexCount);

	// ���� ��ƼŬ ��� �迭���� ���� �迭�� ����ϴ�. �� ��ƼŬ�� �� ���� �ﰢ������ ������� �����Դϴ�.
	int index = 0;

	for (int i = 0; i < currentParticleCount; i++)
	{
		// ���� �Ʒ�.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// ���� ��.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// ������ �Ʒ�.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// ������ �Ʒ�.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// ���� ��.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// ������ ��.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;
	}

	// ���� ���۸� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(Graphic->GetDeviceContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
		 VertexType* verticesPtr = (VertexType*)mappedResource.pData;

		// �����͸� ���� ���ۿ� �����մϴ�.
		memcpy(mappedResource.pData, (void*)vertices.get(), (sizeof(VertexType) * vertexCount));

		// ���� ������ ����� �����մϴ�.
		Graphic->GetDeviceContext()->Unmap(vertexBuffer.Get(), 0);
	}


}

void ParticleSystem::RenderBuffers()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

HRESULT ParticleSystem::Initialize(MATERIALDESC& _materialDesc)
{
	Graphic = GraphicDevice::GetInstance();

	material = Material::Create(_materialDesc);
	isBillboard = true;
	transform = Transform::Create(Transform::TRANSDESC());
	transform->SetState(Transform::POSITION, Vector3(10.f, 2.f, -10.f));
	transform->SetObject(shared_from_this());
	
	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }


	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ParticleVS.hlsl", L"../Engine/ParticlePS.hlsl");


	if (FAILED(InitializeParticleSystem()))
	{
		MSG_BOX("Failed to initialize ParticleSystem.");
		return E_FAIL;
	}

	if (FAILED(InitializeBuffers()))
	{
		MSG_BOX("Failed to initialize ParticleSystem's buffers.");
		return E_FAIL;
	}

	components[ComponentType::MATERIAL] = material;
	components[ComponentType::SHADER] = shader;
	components[ComponentType::TRANSFORM] = transform;

	return S_OK;
}

shared_ptr<ParticleSystem> ParticleSystem::Create(MATERIALDESC& _materialDesc)
{
	shared_ptr<ParticleSystem> Instance(new ParticleSystem());
	if (FAILED(Instance->Initialize(_materialDesc)))
	{
		MSG_BOX("Failed to create ParticleSystem.");
		return nullptr;
	}
	return Instance;
}
