#pragma once

#include "Includes.h"

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();

	void Open(wstring filePath, UINT openOption = CREATE_ALWAYS);
	void Close();

	void WriteBool(bool data);
	void WriteWord(WORD data);
	void WriteInt(int data);
	void WriteUInt(UINT data);
	void WriteFloat(float data);
	void WriteDouble(double data);

	void WriteVector2(const Vector2& data);
	void WriteVector3(const Vector3& data);
	void WriteVector4(const Vector4& data);
	void WriteMatrix(const Matrix& data);

	void WriteString(const string& data);
	void WriteByte(void* data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};

//////////////////////////////////////////////////////////////////////////

class BinaryReader
{
public:
	BinaryReader();
	~BinaryReader();

	void Open(wstring filePath);
	void Close();

	bool ReadBool();
	WORD ReadWord();
	int ReadInt();
	UINT ReadUInt();
	float ReadFloat();
	double ReadDouble();

	Vector2 ReadVector2();
	Vector3 ReadVector3();
	Vector4 ReadVector4();
	Matrix ReadMatrix();

	string ReadString();
	void ReadByte(void** data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};