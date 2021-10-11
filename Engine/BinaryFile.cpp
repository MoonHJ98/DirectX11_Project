#include "pch.h"
#include "BinaryFile.h"

//////////////////////////////////////////////////////////////////////////

BinaryWriter::BinaryWriter()
	: fileHandle(NULL), size(0)
{

}

BinaryWriter::~BinaryWriter()
{

}

void BinaryWriter::Open(wstring filePath, UINT openOption)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_WRITE
		, 0
		, NULL
		, openOption
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

void BinaryWriter::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

void BinaryWriter::WriteBool(bool data)
{
	WriteFile(fileHandle, &data, sizeof(bool), &size, NULL);
}

void BinaryWriter::WriteWord(WORD data)
{
	WriteFile(fileHandle, &data, sizeof(WORD), &size, NULL);
}

void BinaryWriter::WriteInt(int data)
{
	WriteFile(fileHandle, &data, sizeof(int), &size, NULL);
}

void BinaryWriter::WriteUInt(UINT data)
{
	WriteFile(fileHandle, &data, sizeof(UINT), &size, NULL);
}

void BinaryWriter::WriteFloat(float data)
{
	WriteFile(fileHandle, &data, sizeof(float), &size, NULL);
}

void BinaryWriter::WriteDouble(double data)
{
	WriteFile(fileHandle, &data, sizeof(double), &size, NULL);
}

void BinaryWriter::WriteVector2(const Vector2& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector2), &size, NULL);
}

void BinaryWriter::WriteVector3(const Vector3& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector3), &size, NULL);
}

void BinaryWriter::WriteVector4(const Vector4& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector4), &size, NULL);
}

void BinaryWriter::WriteMatrix(const Matrix& data)
{
	WriteFile(fileHandle, &data, sizeof(Matrix), &size, NULL);
}

void BinaryWriter::WriteString(const string & data)
{
	WriteUInt(data.size());

	const char* str = data.c_str();
	WriteFile(fileHandle, str, data.size(), &size, NULL);
}

void BinaryWriter::WriteByte(void * data, UINT dataSize)
{
	WriteFile(fileHandle, data, dataSize, &size, NULL);
}

//////////////////////////////////////////////////////////////////////////

BinaryReader::BinaryReader()
	: fileHandle(NULL), size(0)
{

}

BinaryReader::~BinaryReader()
{

}

void BinaryReader::Open(wstring filePath)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

void BinaryReader::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

bool BinaryReader::ReadBool()
{
	bool temp = false;
	ReadFile(fileHandle, &temp, sizeof(bool), &size, NULL);

	return temp;
}

WORD BinaryReader::ReadWord()
{
	WORD temp = 0;
	ReadFile(fileHandle, &temp, sizeof(WORD), &size, NULL);

	return temp;
}

int BinaryReader::ReadInt()
{
	int temp = 0;
	ReadFile(fileHandle, &temp, sizeof(int), &size, NULL);

	return temp;
}

UINT BinaryReader::ReadUInt()
{
	UINT temp = 0;
	ReadFile(fileHandle, &temp, sizeof(UINT), &size, NULL);

	return temp;
}

float BinaryReader::ReadFloat()
{
	float temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(float), &size, NULL);

	return temp;
}

double BinaryReader::ReadDouble()
{
	double temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(double), &size, NULL);

	return temp;
}

Vector2 BinaryReader::ReadVector2()
{
	float x = ReadFloat();
	float y = ReadFloat();

	return Vector2(x, y);
}

Vector3 BinaryReader::ReadVector3()
{
	float x = ReadFloat();
	float y = ReadFloat();
	float z = ReadFloat();

	return Vector3(x, y, z);
}

Vector4 BinaryReader::ReadVector4()
{
	float x = ReadFloat();
	float y = ReadFloat();
	float z = ReadFloat();
	float w = ReadFloat();

	return Vector4(x, y, z, w);
}

Matrix BinaryReader::ReadMatrix()
{
	Matrix matrix;
	matrix._11 = ReadFloat();	matrix._12 = ReadFloat();	matrix._13 = ReadFloat();	matrix._14 = ReadFloat();
	matrix._21 = ReadFloat();	matrix._22 = ReadFloat();	matrix._23 = ReadFloat();	matrix._24 = ReadFloat();
	matrix._31 = ReadFloat();	matrix._32 = ReadFloat();	matrix._33 = ReadFloat();	matrix._34 = ReadFloat();
	matrix._41 = ReadFloat();	matrix._42 = ReadFloat();	matrix._43 = ReadFloat();	matrix._44 = ReadFloat();

	return matrix;
}

string BinaryReader::ReadString()
{
	UINT size = ReadInt();

	char* temp = new char[size + 1];
	ReadFile(fileHandle, temp, sizeof(char) * size, &this->size, NULL); //데이터 읽기
	temp[size] = '\0';

	return temp;
}

void BinaryReader::ReadByte(void ** data, UINT dataSize)
{
	ReadFile(fileHandle, *data, dataSize, &size, NULL);
}
