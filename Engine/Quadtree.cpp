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

	// �߽� x, z�� �ʺ� ���
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	shared_ptr<NodeType> tempParentNode(new NodeType());
	parentNode = tempParentNode;

	if (parentNode == nullptr)
	{
		MSG_BOX("Failed to create Quadtree's parentNode.");
		return E_FAIL;
	}

	// ���� ��� �����Ϳ� �޽� ������ ������� ����Ʈ���� ��������� �����Ѵ�.
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

	// �޽��� ��� ������ ��ģ��.
	for (UINT i = 0; i< _vertexCount; ++i)
	{
		_centerX += vertexList.get()[i].position.x;
		_centerZ += vertexList.get()[i].position.z;
	}

	// �޽��� �߰� ���� ã�� ���� ������ ���� ������.
	_centerX = _centerX / (float)_vertexCount;
	_centerZ = _centerZ / (float)_vertexCount;

	float maxWidth = 0.f;
	float maxDepth = 0.f;

	float minWidth = fabsf(vertexList.get()[0].position.x - _centerX);
	float minDepth = fabsf(vertexList.get()[0].position.x - _centerZ);

	// ��� ������ ���캸�� �޽��� �ִ� �ʺ�� �ּ� �ʺ�� ���̸� ã�´�.
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

	// �ּҿ� �ִ� ���̿� �ʺ������ ���� �ִ� ���� ã�´�.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// �޽��� �ִ� ������ ����Ѵ�.
	_width = max(maxX, maxZ) * 2.f;

}

void Quadtree::CreateTreeNode(shared_ptr<NodeType>& _node, float _positionX, float _positionZ, float _width)
{
	// ����� ��ġ�� ũ�⸦ ����.
	_node->positionX = _positionX;
	_node->positionZ = _positionZ;
	_node->width = _width;

	// ����� ����� ����  0���� �ʱ�ȭ.
	_node->triangleCount = 0;

	// ���� �� �ε��� ���۸� null�� �ʱ�ȭ
	_node->vertexBuffer.Reset();
	_node->indexBuffer.Reset();

	// �ڽ� ��� �ʱ�ȭ.
	for (int i = 0; i < 4; ++i)
	{
		_node->nodes[i] = nullptr;
	}

	int numTriangles = CountTriangles(_positionX, _positionZ, _width);

	// ��� 1 : �� ��忡 �ﰢ���� ������ ����ִ� ���·� ���ư��� ó�� �� �ʿ䰡 ����.
	if (numTriangles == 0)
		return;

	// ��� 2 : �� ��忡 �ʹ� ���� �ﰢ���� �ִ� ��� 4 ���� ������ ũ���� �� ���� Ʈ���� �����.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; ++i)
		{
			// ���ο� �ڽ� ��忡 ���� ��ġ �������� ���.
			float offsetX = (((i % 2) < 1) ? -1.f : 1.f) * (_width / 4.f);
			float offsetZ = (((i % 4) < 2) ? -1.f : 1.f) * (_width / 4.f);

			// �� ��忡 �ﰢ���� �ִ��� Ȯ��
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

	// ��� 3 : �� ��尡 ������� �ʰ�, �� ����� �ﰢ������ �ִ� �� ���� ������
	// �� ���� ������ �� �Ʒ��� �����Ƿ� ������ �ﰢ�� ����� �����.
	_node->triangleCount = numTriangles;

	//������ ���� ���.
	int vertexCount = numTriangles * 3;

	// ���� �迭 ����
	TerrainVertexType* vertices = new TerrainVertexType[vertexCount];

	ULONG* indices = new ULONG[vertexCount];

	// �� ���ο� ���� �� �ε��� �迭�� �ε����� �ʱ�ȭ�Ѵ�.
	int index = 0;
	int vertexIndex = 0;

	// ���� ����� ��� �ﰢ���� ���캻��.
	for (UINT i = 0; i < triangleCount; ++i)
	{
		// �ﰢ���� �� ���ȿ� ������ ������ �迭�� �߰��Ѵ�.
		if (isTriangleContained(i, _positionX, _positionZ, _width))
		{
			// ���� ���ؽ� ��Ͽ� �ε����� ����Ѵ�.
			vertexIndex = i * 3;

			// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����´�.
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



	// ����, �ε��� ���� ����.
	CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), vertices, vertexCount, sizeof(TerrainVertexType), D3D11_BIND_VERTEX_BUFFER, _node->vertexBuffer.GetAddressOf());
	CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), indices, vertexCount, sizeof(ULONG), D3D11_BIND_INDEX_BUFFER, _node->indexBuffer.GetAddressOf());

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

}

int Quadtree::CountTriangles(float _positionX, float _positionZ, float _width)
{
	// ī��Ʈ�� �ʱ�ȭ�Ѵ�.
	int count = 0;
	
	// ��ü �޽��� ��� �ﰢ���� ���캸�� � ��尡 �� ���ȿ� �ִ��� Ȯ���Ѵ�.
	for (UINT i = 0; i < triangleCount; ++i)
	{
		// �ﰢ���� ���ȿ� ������ 1�� ���� ��Ų��.
		if (isTriangleContained(i, _positionX, _positionZ, _width))
			++count;
	}
	
	return count;
}

bool Quadtree::isTriangleContained(int _index, float _positionX, float _positionZ, float _width)
{
	// �� ����� �ݰ��� ���.
	float radius = _width / 2.f;

	// �ε����� ���� ������� �����´�.
	int vertexIndex = _index * 3;

	// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����´�.
	float x1 = vertexList.get()[vertexIndex].position.x;
	float z1 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	float x2 = vertexList.get()[vertexIndex].position.x;
	float z2 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	float x3 = vertexList.get()[vertexIndex].position.x;
	float z3 = vertexList.get()[vertexIndex].position.z;
	++vertexIndex;

	// �ﰢ����  x��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ѵ�.
	float minimunX = min(x1, min(x2, x3));
	if (minimunX > (_positionX + radius))
		return false;

	// �ﰢ����  x��ǥ�� �ִ밪�� ��� �ȿ� �ִ��� Ȯ���Ѵ�.
	float maximunX = max(x1, max(x2, x3));
	if (maximunX < (_positionX - radius))
		return false;

	// �ﰢ����  z��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ѵ�.
	float minimunZ = min(z1, min(z2, z3));
	if (minimunZ > (_positionZ + radius))
		return false;

	// �ﰢ����  z��ǥ�� �ִ밪�� ��� �ȿ� �ִ��� Ȯ���Ѵ�.
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

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, _node->vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	graphic->GetDeviceContext()->IASetIndexBuffer(_node->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// �� ���ؽ� ���ۿ��� �������Ǿ���ϴ� ������Ƽ���� Ÿ���� �����Ѵ�.�� ��� ���θ���Ʈ�̴�.
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
