#include "pch.h"
#include "DebugTree.h"
#include "GraphicDevice.h"
#include "Shader.h"
#include "Transform.h"
#include "DebugTreeRenderer.h"
#include "Management.h"

DebugTree::DebugTree()
{
}

DebugTree::DebugTree(const DebugTree & Rhs)
{
}

DebugTree::~DebugTree()
{
}

HRESULT DebugTree::Initialize(shared_ptr<NodeType> _rootNode, shared_ptr<TerrainVertexType> _vertices)
{
	if (FAILED(CreateBoundingBox(_rootNode)))
	{
		MSG_BOX("Failed to create BoundingBox for DebugTree.");
		return E_FAIL;
	}

	for (size_t i = 0; i < boundingBox.size(); ++i)
	{

		if (FAILED(CreateDebugTreeRenderer(boundingBox[i])))
		{
			MSG_BOX("Failed to create DebugTreeRenderer for DebugTree.");
			return E_FAIL;
		}
	}

	Manage = Management::GetInstance();

	vertices = _vertices;

	return S_OK;
}

HRESULT DebugTree::CreateBoundingBox(shared_ptr<NodeType> _node)
{
	if (_node == nullptr)
		return S_OK;

	if (!_node->nodes[0])
		boundingBox.push_back(BoundingBox(Vector3(_node->positionX, 0.f, _node->positionZ), Vector3(_node->width / 2.f, 20.f, _node->width / 2.f)));



	for (int i = 0; i < 4; ++i)
		CreateBoundingBox(_node->nodes[i]);

	return S_OK;
}

HRESULT DebugTree::CreateDebugTreeRenderer(BoundingBox & _boundingBox)
{
	auto debugTreeRenderer = DebugTreeRenderer::Create(_boundingBox);

	debugTreeRenderers.push_back(debugTreeRenderer);

	return S_OK;
}


void DebugTree::Render()
{
	for (UINT i = 0; i < debugTreeRenderers.size(); ++i)
	{
		debugTreeRenderers[i]->Render();
	}
}

Vector3 DebugTree::PickTerrain(Vector2 _screenCursorPos, Vector2 _screenSize)
{
	//POINT p;
	//GetCursorPos(&p);
	//ScreenToClient(hWnd, &p);

	Matrix	matProj;
	matProj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);


	// 뷰포트의 마우스를 투영의 마우스로 변환 -> 뷰스페이스로 변환
	Vector3		Temp;

	Temp.x = (float(_screenCursorPos.x) / ((int)_screenSize.x >> 1) - 1.f) / matProj._11;
	Temp.y = (float(-_screenCursorPos.y) / ((int)_screenSize.y >> 1) + 1.f) / matProj._22;
	Temp.z = 1.f;

	// 뷰 스페이스 상의 rayPos, rayDir
	Vector3 RayPos = Vector3(0.f, 0.f, 0.f);
	Vector3 RayDir = Temp - RayPos;
	RayDir.Normalize();

	// 월드로 변환
	Matrix	matView;
	matView = *Manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	//Matrix world = *transform->GetWorldMatrix();
	//Matrix WorldView = world * matView;
	//Matrix invmatrix = WorldView.Invert();
	matView = matView.Invert();


	Vector3::Transform(RayPos, matView, RayPos);
	Vector3::TransformNormal(RayDir, matView, RayDir);

	Ray ray;
	ray.position = RayPos;
	ray.direction = RayDir;

	float dist = 0.f;



	for (UINT i = 0; i < boundingBox.size(); ++i)
	{
		Vector3 Pos;

		if (ray.Intersects(boundingBox[i], dist))
		{

			UINT terrainHeight = (UINT)(boundingBox[i].Center.z + boundingBox[i].Extents.z);
			UINT terrainWidth = (UINT)(boundingBox[i].Center.x + boundingBox[i].Extents.x);

			UINT terrainStartZ = (UINT)(boundingBox[i].Center.z - boundingBox[i].Extents.z);
			UINT terrainStartX = (UINT)(boundingBox[i].Center.x - boundingBox[i].Extents.x);

			for (UINT z = terrainStartZ; z < terrainHeight; ++z)
			{
				for (UINT x = terrainStartX; x < terrainWidth; ++x)
				{
					int	index[4] = { -1 };

					index[0] = (terrainHeight * z) + x;
					index[1] = (terrainHeight * (z + 1)) + x;
					index[2] = (terrainHeight * z) + x + 1;
					index[3] = (terrainHeight * (z +  1)) + (x + 1);

					
					Vector3 vertex1 = vertices.lock().get()[index[0]].position;
					Vector3 vertex2 = vertices.lock().get()[index[1]].position;
					Vector3 vertex3 = vertices.lock().get()[index[2]].position;
					Vector3 vertex4 = vertices.lock().get()[index[3]].position;


					//cout << index[0] << " , "<< index[1] << " , " << index[2] << " , "<<index[3]<< endl;

					if(index[0] == -1)
						return Pos;
					if (ray.Intersects(vertex1, vertex2, vertex3, dist))
					{
						Pos = ray.position + ray.direction * dist;
						//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
						return Pos;
					}
					if (ray.Intersects(vertex4, vertex3, vertex2, dist))
					{
						Pos = ray.position + ray.direction * dist;
						//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
						return Pos;
					}
				}
			}
			return Pos;
		}
	}

	return Vector3(0.f, 0.f, 0.f);
}

shared_ptr<DebugTree> DebugTree::Create(shared_ptr<NodeType> _rootNode, shared_ptr<TerrainVertexType> _vertices)
{
	shared_ptr<DebugTree> Instance(new DebugTree());
	if (FAILED(Instance->Initialize(_rootNode, _vertices)))
	{
		MSG_BOX("Failed to create DebugTree.");
		return nullptr;
	}
	return Instance;
}
