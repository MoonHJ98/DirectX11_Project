#include "pch.h"
#include "Quadtree.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "Frustum.h"
#include "GraphicDevice.h"
#include "TerrainBufferTest.h"


Quadtree::Quadtree()
{
}

Quadtree::Quadtree(const Quadtree & Rhs)
{
}

Quadtree::~Quadtree()
{
}

int Quadtree::Update(float _timeDelta)
{
	return 0;
}

void Quadtree::Render()
{
	RenderNode(parentNode);
}

void Quadtree::RenderInspector()
{
}

HRESULT Quadtree::Initialize(shared_ptr<TerrainBufferTest> _terrainBuffer)
{
	manage = Management::GetInstance();
	graphic = GraphicDevice::GetInstance();

	float centerX = 0.f;
	float centerZ = 0.f;
	float width = 0.f;

	UINT vertexCount = _terrainBuffer->GetVertexCount();

	triangleCount = vertexCount / 3;

	shared_ptr<TerrainVertexType>tempVertexList(new TerrainVertexType[vertexCount]);
	vertexList = tempVertexList;

	if (vertexList == nullptr)
	{
		MSG_BOX("Failed to create Quadtree's vertesList.");
		return E_FAIL;
	}

	_terrainBuffer->CopyVertexArray(vertexList.get());

	// 중심 x, z와 너비를 계산
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	shared_ptr<NodeType> tempParentNode(new NodeType());
	parentNode = tempParentNode;

	if (parentNode == nullptr)
	{
		MSG_BOX("Failed to create Quadtree's parentNode.");
		return E_FAIL;
	}

	// 정점 목록 데이터와 메쉬 차원을 기반으로 쿼드트리를 재귀적으로 생성한다.
	CreateTreeNode(parentNode, centerX, centerZ, width);

	if (vertexList)
	{
		vertexList.reset();
		vertexList = nullptr;
	}

	return S_OK;
}

void Quadtree::CalculateMeshDimensions(UINT _vertexCount, float& _centerX, float& _centerZ, float& _width)
{
	_centerX = 0.f;
	_centerZ = 0.f;

	// 메쉬의 모든 정점을 합친다.
	for (UINT i = 0; i< _vertexCount; ++i)
	{
		_centerX += vertexList.get()[i].position.x;
		_centerZ += vertexList.get()[i].position.z;
	}

	// 메쉬의 중간 점을 찾기 위해 정점의 수로 나눈다.
	_centerX = _centerX / (float)_vertexCount;
	_centerZ = _centerZ / (float)_vertexCount;

	float maxWidth = 0.f;
	float maxDepth = 0.f;

	float minWidth = fabsf(vertexList.get()[0].position.x - _centerX);
	float minDepth = fabsf(vertexList.get()[0].position.x - _centerZ);

	// 모든 정점을 살펴보고 메쉬의 최대 너비와 최소 너비와 깊이를 찾는다.
	for (UINT i = 0; i < _vertexCount; ++i)
	{
		float width = fabs(vertexList.get()[i].position.x - _centerX);
		float depth = fabs(vertexList.get()[i].position.z - _centerZ);

		if (width > maxWidth)
			maxWidth = width;
		if (depth > maxDepth)
			maxDepth = depth;
		if (width < minWidth)
			minWidth = width;
		if (depth < minDepth)
			minDepth = depth;
	}

	// 최소와 최대 깊이와 너비사이의 절대 최대 값을 찾는다.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// 메쉬의 최대 직경을 계산한다.
	_width = max(maxX, maxZ) * 2.f;

}

void Quadtree::CreateTreeNode(shared_ptr<NodeType>& _node, float _positionX, float _positionZ, float _width)
{
	// 노드의 위치와 크기를 저장.
	_node->positionX = _positionX;
	_node->positionZ = _positionZ;
	_node->width = _width;

	// 노드의 삼격형 수를  0으로 초기화.
	_node->triangleCount = 0;

	// 정점 및 인덱스 버퍼를 null로 초기화
	_node->vertexBuffer.Reset();
	_node->indexBuffer.Reset();

	// 자식 노드 초기화.
	for (int i = 0; i < 4; ++i)
	{
		_node->nodes[i] = nullptr;
	}

	int numTriangles = CountTriangles(_positionX, _positionZ, _width);

	// 사례 1 : 이 노드에 삼각형이 없으면 비어있는 상태로 돌아가서 처리 할 필요가 없다.
	if (numTriangles == 0)
		return;

	// 사례 2 : 이 노드에 너무 많은 삼각형이 있는 경우 4 개의 동일한 크기의 더 작은 트리를 만든다.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; ++i)
		{
			// 새로운 자식 노드에 대한 위치 오프셋을 계산.
			float offsetX = (((i % 2) < 1) ? -1.f : 1.f) * (_width / 4.f);
			float offsetZ = (((i % 4) < 2) ? -1.f : 1.f) * (_width / 4.f);

			// 새 노드에 삼각형이 있는지 확인
			int count = CountTriangles((_positionX + offsetX), (_positionZ + offsetZ), (_width/ 2.f));

			if (count > 0)
			{
				shared_ptr<NodeType> tempNode(new NodeType());
				_node->nodes[i] = tempNode;
				CreateTreeNode(_node->nodes[i], (_positionX + offsetX), (_positionZ + offsetZ), (_width / 2.f));

			}
	
		}
		return;
	}

	// 사례 3 : 이 노드가 비어있지 않고, 그 노드의 삼각형수가 최대 값 보다 작으면
	// 이 노드는 ㅌ리의 맨 아래에 있으므로 저장할 삼각형 목록을 만든다.
	_node->triangleCount = numTriangles;

	//정점의 수를 계산.
	int vertexCount = numTriangles * 3;

	// 정점 배열 생성
	TerrainVertexType* vertices = new TerrainVertexType[vertexCount];

	ULONG* indices = new ULONG[vertexCount];

	// 이 새로운 정점 및 인덱스 배열의 인덱스를 초기화한다.
	int index = 0;
	int vertexIndex = 0;

	// 정점 목록의 모든 삼각형을 살펴본다.
	for (UINT i = 0; i < triangleCount; ++i)
	{
		// 삼각형이 이 노드안에 있으면 꼭지점 배열에 추가한다.
		if (isTriangleContained(i, _positionX, _positionZ, _width))
		{
			// 지형 버텍스 목록에 인덱스를 계산한다.
			vertexIndex = i * 3;

			// 지형 목록에서 이 삼각형의 세 꼭지점을 가져온다.
			vertices[index].position = vertexList.get()[vertexIndex].position;
			vertices[index].normal = vertexList.get()[vertexIndex].normal;
			vertices[index].Uv = vertexList.get()[vertexIndex].Uv;
			indices[index] = index;
			++index;

			++vertexIndex;
			vertices[index].position = vertexList.get()[vertexIndex].position;
			vertices[index].normal = vertexList.get()[vertexIndex].normal;
			vertices[index].Uv = vertexList.get()[vertexIndex].Uv;
			indices[index] = index;
			++index;

			++vertexIndex;
			vertices[index].position = vertexList.get()[vertexIndex].position;
			vertices[index].normal = vertexList.get()[vertexIndex].normal;
			vertices[index].Uv = vertexList.get()[vertexIndex].Uv;
			indices[index] = index;
			++index;
		}
	}



	// 정점, 인덱스 버퍼 생성.
	CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), vertices, vertexCount, sizeof(TerrainVertexType), D3D11_BIND_VERTEX_BUFFER, _node->vertexBuffer.GetAddressOf());
	CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), indices, vertexCount, sizeof(ULONG), D3D11_BIND_INDEX_BUFFER, _node->indexBuffer.GetAddressOf());

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

}

int Quadtree::CountTriangles(float _positionX, float _positionZ, float _width)
{
	// 카운트를 초기화한다.
	int count = 0;
	
	// 전체 메쉬의 모든 삼각형을 살펴보고 어떤 노드가 이 노드안에 있는지 확인한다.
	for (UINT i = 0; i < triangleCount; ++i)
	{
		// 삼각형이 노드안에 있으면 1씩 증가 시킨다.
		if (isTriangleContained(i, _positionX, _positionZ, _width))
			++count;
	}
	
	return count;
}

bool Quadtree::isTriangleContained(int _index, float _positionX, float _positionZ, float _width)
{
	// 이 노드의 반경을 계산.
	float radius = _width / 2.f;

	// 인덱스를 정점 목록으로 가져온다.
	int vertexIndex = _index * 3;

	// 정점 목록에서 이 삼각형의 세 꼭지점을 가져온다.
	float x1 = vertexList.get()[vertexIndex].position.x;
	float z1 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	float x2 = vertexList.get()[vertexIndex].position.x;
	float z2 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	float x3 = vertexList.get()[vertexIndex].position.x;
	float z3 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	// 삼각형의  x좌표의 최소값이 노드 안에 있는지 확인한다.
	float minimunX = min(x1, min(x2, x3));
	if (minimunX > (_positionX + radius))
		return false;

	// 삼각형의  x좌표의 최대값이 노드 안에 있는지 확인한다.
	float maximunX = max(x1, max(x2, x3));
	if (maximunX < (_positionX - radius))
		return false;

	// 삼각형의  z좌표의 최소값이 노드 안에 있는지 확인한다.
	float minimunZ = min(z1, min(z2, z3));
	if (minimunZ > (_positionZ + radius))
		return false;

	// 삼각형의  z좌표의 최대값이 노드 안에 있는지 확인한다.
	float maximunZ = max(z1, max(z2, z3));
	if (maximunZ < (_positionZ - radius))
		return false;

	return true;
}

void Quadtree::RenderNode(shared_ptr<NodeType> _node)
{
	auto frustum = manage->GetFrustum();

	if (frustum == nullptr)
		return;

	if(!frustum->CheckCube(Vector3(_node->positionX, 0.f, _node->positionZ), _node->width / 2.f))
		return;

	int count = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (_node->nodes[i] != nullptr)
		{
			++count;
			RenderNode(_node->nodes[i]);
		}
	}

	if (count != 0)
		return;


	UINT stride = sizeof(TerrainVertexType);
	UINT offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, _node->vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	graphic->GetDeviceContext()->IASetIndexBuffer(_node->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int indexCount = _node->triangleCount * 3;

	graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

shared_ptr<Quadtree> Quadtree::Create(shared_ptr<TerrainBufferTest> _terrainBuffer)
{
	shared_ptr<Quadtree> Instance(new Quadtree());
	if (FAILED(Instance->Initialize(_terrainBuffer)))
	{
		MSG_BOX("Failed to create Quadtree.");
		return nullptr;
	}
	return Instance;
}
