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

	// 오래된 파티클을 해제합니다.
	KillParticles();

	// 새 파티클을 방출합니다.
	EmitParticles(_TimeDelta);

	// 파티클 위치를 업데이트 합니다.
	UpdateParticles(_TimeDelta);

	// 동적 정점 버퍼를 각 파티클의 새 위치로 업데이트합니다.
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
	// 방출 될 때 파티클이 위치 할 수 있는 임이의 편차를 설정.
	particleDeviation = Vector3(0.5f, 0.1f, 2.f);

	// 파티클의 속도와 속도 변화를 설정.
	particleVelocity = 1.f;
	particleVelocityVariation = 0.2f;

	// 파티클의 물리적 크기 설정
	particleSize = 10.f;

	// 초당 방출 할 파티클 수 설정.
	particlePerSecond = 250.f;

	// 파티클 시스템에 허용되는 푀대 파티클 수를 결정
	maxParticles = 5000;

	// 파티클 리스트 생성.
	shared_ptr<ParticleType> tempParticleList(new ParticleType[maxParticles]);
	particleList = tempParticleList;
	if (particleList == nullptr)
	{
		MSG_BOX("Failed to create Particle List.");
		return E_FAIL;
	}

	// 아직 방출되지 않은 현재 파티클 수를 0으로 초기화.
	currentParticleCount = 0;

	// 초당 파티클 방출 속도의 초기 누적값을 0으로 초기화.
	accumulatedTime = 0.f;

	return S_OK;
}

HRESULT ParticleSystem::InitializeBuffers()
{
	vertexCount = maxParticles * 6;

	indexCount = vertexCount;

	// 렌더링 될 파티클에 대한 정점 배열 생성.
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
	// 프레임 시간을 증가시킵니다.
	accumulatedTime += _timeDelta;

	// 이제 파티클 출력을 false로 설정합니다.
	bool emitParticle = false;

	// 새 파티클을 방출할 시간인지 확인합니다.
	if (accumulatedTime > (1000.0f / particlePerSecond))
	{
		accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// 방출 할 파티클이 있으면 프레임 당 하나씩 방출합니다.
	if ((emitParticle == true) && (currentParticleCount < (maxParticles - 1)))
	{
		currentParticleCount++;

		// 이제 임의 화 된 파티클 속성을 생성합니다.
		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.x;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.y;
		float positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviation.z;

		Vector3 _position = Vector3(positionX, positionY, positionZ);

		float velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;

		float red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		Vector3 _color = Vector3(red, green, blue);

		// 이제 블렌딩을 위해 파티클을 뒤에서 앞으로 렌더링해야하므로 파티클 배열을 정렬해야 합니다.
		// Z 깊이를 사용하여 정렬하므로 목록에서 파티클을 삽입해야 하는 위치를 찾아야합니다.
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

		// 삽입 할 위치를 알았으므로 인덱스에서 한 위치 씩 배열을 복사하여 새 파티클을 위한 공간을 만들어야합니다.
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

		// 이제 정확한 깊이 순서로 파티클 배열에 삽입하십시오.
		particleList.get()[index].position = _position;
		particleList.get()[index].color = _color;
		particleList.get()[index].velocity = velocity;
		particleList.get()[index].active = true;
	}
}

void ParticleSystem::UpdateParticles(float _timeDelta)
{
	// 각 프레임은 위치, 속도 및 프레임 시간을 사용하여 아래쪽으로 이동하여 모든 파티클을 업데이트합니다.
	for (int i = 0; i < currentParticleCount; i++)
	{
		particleList.get()[i].position.y = particleList.get()[i].position.y - (particleList.get()[i].velocity * _timeDelta);
	}
}

void ParticleSystem::KillParticles()
{
	// 특정 높이 범위를 벗어난 모든 파티클을 제거합니다.
	for (int i = 0; i < maxParticles; i++)
	{
		if ((particleList.get()[i].active == true) && (particleList.get()[i].position.y < -3.0f))
		{
			particleList.get()[i].active = false;
			currentParticleCount--;

			// 이제 모든 살아있는 파티클을 배열위로 이동시켜 파괴된 파티클을 지우고 배열을 올바르게 정렬된 상태로 유지합니다.
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
	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices.get(), 0, sizeof(VertexType) * vertexCount);

	// 이제 파티클 목록 배열에서 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 만들어진 쿼드입니다.
	int index = 0;

	for (int i = 0; i < currentParticleCount; i++)
	{
		// 왼쪽 아래.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// 왼쪽 위.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// 오른쪽 아래.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// 오른쪽 아래.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y - particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 1.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// 왼쪽 위.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x - particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(0.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;

		// 오른쪽 위.
		vertices.get()[index].position = Vector3(particleList.get()[i].position.x + particleSize, particleList.get()[i].position.y + particleSize, particleList.get()[i].position.z);
		vertices.get()[index].texture = Vector2(1.0f, 0.0f);
		vertices.get()[index].color = Vector3(particleList.get()[i].color);
		index++;
	}

	// 정점 버퍼를 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(Graphic->GetDeviceContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
		 VertexType* verticesPtr = (VertexType*)mappedResource.pData;

		// 데이터를 정점 버퍼에 복사합니다.
		memcpy(mappedResource.pData, (void*)vertices.get(), (sizeof(VertexType) * vertexCount));

		// 정점 버퍼의 잠금을 해제합니다.
		Graphic->GetDeviceContext()->Unmap(vertexBuffer.Get(), 0);
	}


}

void ParticleSystem::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다.
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
