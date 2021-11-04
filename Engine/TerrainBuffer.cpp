#include "pch.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"

TerrainBuffer::TerrainBuffer()
{
}

TerrainBuffer::TerrainBuffer(const TerrainBuffer & Rhs)
{
}

TerrainBuffer::~TerrainBuffer()
{
	delete heightMap;
	heightMap = nullptr;
}

HRESULT TerrainBuffer::Initialize(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	Graphic = GraphicDevice::GetInstance();

	//terrainWidth = _terrainWidth;
	//terrainHeight = _terrainHeight;

	if (_heightMapPath != L"")
	{
		if (FAILED(LoadHeightMap(_heightMapPath)))
		{
			MSG_BOX("Failed to load height map.");
			return E_FAIL;
		}
		SetTerrainCoordinates();
	}

	if (FAILED(CalculateNormal()))
	{
		MSG_BOX("Failed to calculate normal.");
		return E_FAIL;
	}

	if (FAILED(BuildTerrainModel()))
	{
		MSG_BOX("Failed to create terrain model.");
		return E_FAIL;
	}

	delete heightMap;
	heightMap = nullptr;


	if (FAILED(InitializeBuffer()))
	{
		MSG_BOX("Failed to initial buffer for TerrainBuffer.");
		return E_FAIL;
	}

	delete terrainModel;
	terrainModel = nullptr;

	return S_OK;
}

HRESULT TerrainBuffer::InitializeBuffer()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	UINT i;
	XMFLOAT4 color;


	// Set the color of the terrain grid.
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate the number of vertices in the terrain.
	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	indexCount = vertexCount;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with 3D terrain model data.
	for (i = 0; i < vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(terrainModel[i].x, terrainModel[i].y, terrainModel[i].z);
		vertices[i].uv = XMFLOAT2(terrainModel[i].u, terrainModel[i].v);
		vertices[i].normal = XMFLOAT3(terrainModel[i].nx, terrainModel[i].ny, terrainModel[i].nz);
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = Graphic->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = Graphic->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	return S_OK;
}

HRESULT TerrainBuffer::LoadHeightMap(wstring _heightMapPath)
{
	int error, imageSize;
	UINT i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;


	// Start by creating the array structure to hold the height map data.
	heightMap = new HeightMapType[terrainWidth * terrainHeight];
	if (!heightMap)
	{
		return E_FAIL;
	}

	// Open the bitmap map file in binary.
	error = _wfopen_s(&filePtr, _heightMapPath.c_str(), L"rb");
	if (error != 0)
	{
		return E_FAIL;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return E_FAIL;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return E_FAIL;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != terrainHeight) || (bitmapInfoHeader.biWidth != terrainWidth))
	{
		return E_FAIL;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = terrainHeight * ((terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return E_FAIL;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return E_FAIL;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j < terrainHeight; j++)
	{
		for (i = 0; i < terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (terrainWidth * (terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			heightMap[index].y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = nullptr;


	return S_OK;
}

void TerrainBuffer::SetTerrainCoordinates()
{
	UINT i, j, index;


	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j < terrainHeight; j++)
	{
		for (i = 0; i < terrainWidth; i++)
		{
			index = (terrainWidth * j) + i;

			// Set the X and Z coordinates.
			heightMap[index].x = (float)i;
			heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			heightMap[index].z += (float)(terrainHeight - 1);

			// Scale the height.
			heightMap[index].y /= heightScale;
		}
	}
}

HRESULT TerrainBuffer::BuildTerrainModel()
{
	UINT i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the 3D terrain model.
	vertexCount = (terrainHeight - 1) * (terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	terrainModel = new ModelType[vertexCount];
	if (!terrainModel)
	{
		return false;
	}

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j < (terrainHeight - 1); j++)
	{
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (terrainWidth * j) + i;          // Upper left.
			index2 = (terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			terrainModel[index].x = heightMap[index1].x;
			terrainModel[index].y = heightMap[index1].y;
			terrainModel[index].z = heightMap[index1].z;
			terrainModel[index].u = 0.f;
			terrainModel[index].v = 0.f;
			terrainModel[index].nx = heightMap[index1].nx;
			terrainModel[index].ny = heightMap[index1].ny;
			terrainModel[index].nz = heightMap[index1].nz;
			index++;

			// Triangle 1 - Upper right.
			terrainModel[index].x = heightMap[index2].x;
			terrainModel[index].y = heightMap[index2].y;
			terrainModel[index].z = heightMap[index2].z;
			terrainModel[index].u = 1.f;
			terrainModel[index].v = 0.f;
			terrainModel[index].nx = heightMap[index2].nx;
			terrainModel[index].ny = heightMap[index2].ny;
			terrainModel[index].nz = heightMap[index2].nz;
			index++;

			// Triangle 1 - Bottom left.
			terrainModel[index].x = heightMap[index3].x;
			terrainModel[index].y = heightMap[index3].y;
			terrainModel[index].z = heightMap[index3].z;
			terrainModel[index].u = 0.f;
			terrainModel[index].v = 1.f;
			terrainModel[index].nx = heightMap[index3].nx;
			terrainModel[index].ny = heightMap[index3].ny;
			terrainModel[index].nz = heightMap[index3].nz;
			index++;

			// Triangle 2 - Bottom left.
			terrainModel[index].x = heightMap[index3].x;
			terrainModel[index].y = heightMap[index3].y;
			terrainModel[index].z = heightMap[index3].z;
			terrainModel[index].u = 0.f;
			terrainModel[index].v = 1.f;
			terrainModel[index].nx = heightMap[index3].nx;
			terrainModel[index].ny = heightMap[index3].ny;
			terrainModel[index].nz = heightMap[index3].nz;
			index++;

			// Triangle 2 - Upper right.
			terrainModel[index].x = heightMap[index2].x;
			terrainModel[index].y = heightMap[index2].y;
			terrainModel[index].z = heightMap[index2].z;
			terrainModel[index].u = 1.f;
			terrainModel[index].v = 0.f;
			terrainModel[index].nx = heightMap[index2].nx;
			terrainModel[index].ny = heightMap[index2].ny;
			terrainModel[index].nz = heightMap[index2].nz;
			index++;

			// Triangle 2 - Bottom right.
			terrainModel[index].x = heightMap[index4].x;
			terrainModel[index].y = heightMap[index4].y;
			terrainModel[index].z = heightMap[index4].z;
			terrainModel[index].u = 1.f;
			terrainModel[index].v = 1.f;
			terrainModel[index].nx = heightMap[index4].nx;
			terrainModel[index].ny = heightMap[index4].ny;
			terrainModel[index].nz = heightMap[index4].nz;
			index++;
		}
	}
	return S_OK;
}

HRESULT TerrainBuffer::CalculateNormal()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the face normal vectors.
	normals = new VectorType[(terrainHeight - 1) * (terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (terrainHeight - 1); j++)
	{
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * terrainWidth) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1[0] = heightMap[index1].x;
			vertex1[1] = heightMap[index1].y;
			vertex1[2] = heightMap[index1].z;

			vertex2[0] = heightMap[index2].x;
			vertex2[1] = heightMap[index2].y;
			vertex2[2] = heightMap[index2].z;

			vertex3[0] = heightMap[index3].x;
			vertex3[1] = heightMap[index3].y;
			vertex3[2] = heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j < terrainHeight; j++)
	{
		for (i = 0; i < terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap[index].nx = (sum[0] / length);
			heightMap[index].ny = (sum[1] / length);
			heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = nullptr;

	return S_OK;
}


void TerrainBuffer::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

void TerrainBuffer::Render()
{
	RenderBuffers();
}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight, _heightMapPath)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}

	return Instance;
}
