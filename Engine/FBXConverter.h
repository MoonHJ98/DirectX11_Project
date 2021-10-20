#pragma once

#include "Includes.h"


class FBXConverter
{
public:
	FBXConverter();
	FBXConverter(const FBXConverter& Rhs);
	~FBXConverter();

public:
	HRESULT				ReadFile(wstring _File);
	void				ExportMesh(wstring _SavePath);
	void				ProcessSkeletonHierarchy(FbxNode* RootNode);
	void				ProcessSkeletonHierarchyRecursively(FbxNode* Node, int Depth, int myIndex, int ParentIndex);
	void				ProcessGeometry(FbxNode* Node);
	void				ProcessControlPoints(FbxNode* Node);
	void				ProcessMesh(FbxNode* Node);
	void				ProcessJointsAndAnimations(FbxNode* Node);
	void				ReadNormal(FbxMesh* Mesh, int CtrlPointIndex, int VertexCounter, Vector3& Normal);
	void				ReadTangent(FbxMesh* Mesh, int CtrlPointIndex, int VertexCounter, Vector3& Tangent);
	void				ReadUV(FbxMesh* Mesh, int CtrlPointIndex, int TextureUVIndex, int UVLayer, Vector2& UV);
	UINT				FindJointIndexUsingName(const string& inJointName);


private:
	FbxManager* FbxSdkManager = nullptr;
	FbxScene* fbxScene = nullptr;
	wstring File;
	SEKELETON Skeleton;
	bool HasAnimation = true;
	unordered_map<UINT, CtrlPoint*> ControlPoints;
	UINT TriangleCount = 0;
	vector<TRIANGLE> Triangles;
	vector<PNTIWVertex> Vertices;
	FbxLongLong mAnimationLength;
	std::string mAnimationName;



};

