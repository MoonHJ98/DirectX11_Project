#include "pch.h"
#include "Capsule.h"


void Capsule::CreateCapsule(vector<GeometricPrimitive::VertexType>& vertices, vector<uint16_t>& indices, float radius, float height, Axis axis, int stackCount, int sliceCount)
{
	int lineCount = 0;
	auto lines = CreateVertex(radius, height, axis, stackCount, sliceCount, lineCount);
	
	if (lines == nullptr)
		return;

	//if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
	//	vertexCount = lineCount * 2;
	//else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
	//	vertexCount = lineCount + 1;

	int vertexCount = lineCount * 2;
	vertices.resize(vertexCount);

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = lines[i];
		vertices[i].textureCoordinate = Vector2();
		vertices[i].normal = Vector3(0.f, 1.f, 0.f);
	}

}

Vector3* Capsule::CreateVertex(float radius, float height, Axis axis, int stackCount, int sliceCount, int& lineCount)
{
	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	int vertexCount = 2 + (sliceCount + 1) * (stackCount - 1);

	Vector3* tempVertices = new Vector3[vertexCount];


	int index = 0;
	for (int i = 0; i <= stackCount; i++)
	{

		switch (axis)
		{
		case Capsule::Axis::Axis_X:
		{
			// 위와 밑의 꼭지점 처리
			if (i == 0 || i == stackCount)
			{
				tempVertices[index] = Vector3(i == 0 ? radius + 0.5f * height : height * -0.5f - radius, 0, 0);

				index++;
			}
			else if (i <= stackCount / 2)
			{
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * cosf(phi) + 0.5f * height), (radius * sinf(phi) * cosf(theta)), (radius * sinf(phi) * sinf(theta)));

					index++;
				}
			}
			else
			{
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * cosf(phi) - 0.5f * height), (radius * sinf(phi) * cosf(theta)), (radius * sinf(phi) * sinf(theta)));

					index++;
				}
			}
		}
		break;
		case Axis::Axis_Y:
		{
			// 위와 밑의 꼭지점 처리
			if (i == 0 || i == stackCount)
			{
				tempVertices[index] = Vector3(0, i == 0 ? radius + 0.5f * height : height * -0.5f - radius, 0);

				index++;
			}
			else if (i <= stackCount / 2)
			{
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * sinf(phi) * cosf(theta)), (radius * cosf(phi) + 0.5f * height), (radius * sinf(phi) * sinf(theta)));

					index++;
				}
			}
			else {
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * sinf(phi) * cosf(theta)), (radius * cosf(phi) - 0.5f * height), (radius * sinf(phi) * sinf(theta)));

					index++;
				}
			}
		}
		break;
		case Axis::Axis_Z:
		{
			// 위와 밑의 꼭지점 처리
			if (i == 0 || i == stackCount)
			{
				tempVertices[index] = Vector3(0, 0, i == 0 ? radius + 0.5f * height : height * -0.5f - radius);

				index++;
			}
			else if (i <= stackCount / 2)
			{
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * sinf(phi) * cosf(theta)), (radius * sinf(phi) * sinf(theta)), (radius * cosf(phi) + 0.5f * height));

					index++;
				}
			}
			else {
				float phi = i * phiStep;
				for (int j = 0; j <= sliceCount; j++)
				{
					float theta = j * thetaStep;
					tempVertices[index] = Vector3((radius * sinf(phi) * cosf(theta)), (radius * sinf(phi) * sinf(theta)), (radius * cosf(phi) - 0.5f * height));

					index++;
				}
			}
		}
		break;
		}


	}

	lineCount = (2 + (stackCount - 2) + (stackCount - 1)) * sliceCount;
	Vector3* lines = new Vector3[lineCount * 2];

	index = 0;
	// 윗면
	for (int i = 1; i <= sliceCount; i++) {
		lines[index++] = tempVertices[0];
		lines[index++] = tempVertices[i];
	}

	// 옆면
	int baseIndex = 1; // 위 꼭지점 빼려고
	int ringVertexCount = sliceCount + 1;
	// stackCount - 2 인 이유 정점 4개 써서 위 꼭지 아래꼭지 빼고 -1 해줘야함
	for (int i = 0; i < stackCount - 1; i++) {

		for (int j = 0; j < sliceCount; j++) {
			// 세로줄
			if (i < stackCount - 2)
			{
				lines[index++] = tempVertices[baseIndex + i * ringVertexCount + j];
				lines[index++] = tempVertices[baseIndex + (i + 1) * ringVertexCount + j];
			}

			// 가로줄
			lines[index++] = tempVertices[baseIndex + i * ringVertexCount + j];
			lines[index++] = tempVertices[baseIndex + i * ringVertexCount + (j + 1)];
		}
	}

	// 아랫면
	int southPoleIndex = vertexCount - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (int i = 0; i < sliceCount; i++) 
	{
		lines[index++] = tempVertices[southPoleIndex];
		lines[index++] = tempVertices[baseIndex + i];
	}

	SAFEDELETEARRAY(tempVertices);

	return lines;
}
