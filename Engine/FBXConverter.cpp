#include "pch.h"
#include "FBXConverter.h"

FBXConverter::FBXConverter()
{
}

FBXConverter::FBXConverter(const FBXConverter & Rhs)
{
}

FBXConverter::~FBXConverter()
{
}

HRESULT FBXConverter::ReadFile(wstring _File)
{
	this->File = L"../Resources/" + _File;

	if (FbxSdkManager == nullptr)
	{
		FbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(FbxSdkManager, IOSROOT);
		FbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* Importer = FbxImporter::Create(FbxSdkManager, "");
	fbxScene = FbxScene::Create(FbxSdkManager, "");

	if (false == Importer->Initialize(ToString(File).c_str(), -1, FbxSdkManager->GetIOSettings()))
	{
		MSG_BOX("Failed to read Fbx file.");
		return S_OK;
	}

	if (false == Importer->Import(fbxScene))
	{
		MSG_BOX("Failed to import fbxScene.");
		return E_FAIL;
	}

	Importer->Destroy();


	return S_OK;
}

void FBXConverter::ExportMesh(wstring _SavePath)
{
	_SavePath = L"../Resources/" + _SavePath + L".mesh";

	ProcessSkeletonHierarchy(fbxScene->GetRootNode());

	if (Skeleton.Joints.empty())
		HasAnimation = false;

	ProcessGeometry(fbxScene->GetRootNode());

}

void FBXConverter::ProcessSkeletonHierarchy(FbxNode * RootNode)
{
	for (int childIndex = 0; childIndex < RootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = RootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void FBXConverter::ProcessSkeletonHierarchyRecursively(FbxNode * Node, int Depth, int myIndex, int ParentIndex)
{
	if (Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.mParentIndex = ParentIndex;
		currJoint.mName = Node->GetName();
		Skeleton.Joints.push_back(currJoint);
	}
	for (int i = 0; i < Node->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(Node->GetChild(i), Depth + 1, Skeleton.Joints.size(), myIndex);
	}
}

void FBXConverter::ProcessGeometry(FbxNode * Node)
{
	if (Node->GetNodeAttribute())
	{
		switch (Node->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ProcessControlPoints(Node);
			if (HasAnimation)
			{
				ProcessJointsAndAnimations(Node);
			}
			ProcessMesh(Node);
			//AssociateMaterialToMesh(Node);
			//ProcessMaterials(Node);
			break;
		}
	}

	for (int i = 0; i < Node->GetChildCount(); ++i)
	{
		ProcessGeometry(Node->GetChild(i));
	}
}

void FBXConverter::ProcessControlPoints(FbxNode * Node)
{
	FbxMesh* currMesh = Node->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		ControlPoints[i] = currCtrlPoint;
	}
}

void FBXConverter::ProcessMesh(FbxNode * Node)
{
	FbxMesh* currMesh = Node->GetMesh();

	TriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	Triangles.reserve(TriangleCount);


	for (unsigned int i = 0; i < TriangleCount; ++i)
	{
		Vector3 normal[3];
		Vector3 tangent[3];
		Vector3 binormal[3];
		Vector2 UV[3][2];
		TRIANGLE currTriangle;
		Triangles.push_back(currTriangle);

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
			CtrlPoint* currCtrlPoint = ControlPoints[ctrlPointIndex];


			ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
			// We only have diffuse texture
			for (int k = 0; k < 1; ++k)
			{
				ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
			}


			PNTIWVertex temp;
			temp.mPosition = currCtrlPoint->mPosition;
			temp.mNormal = normal[j];
			temp.mUV = UV[j][0];
			// Copy the blending info from each control point
			for (unsigned int i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
			{
				VertexBlendingInfo currBlendingInfo;
				currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
				currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
				temp.mVertexBlendingInfos.push_back(currBlendingInfo);
			}
			// Sort the blending info so that later we can remove
			// duplicated vertices
			temp.SortBlendingInfoByWeight();

			Vertices.push_back(temp);
			Triangles.back().mIndices.push_back(vertexCounter);
			++vertexCounter;
		}
	}

	// Now mControlPoints has served its purpose
	// We can free its memory
	for (auto itr = ControlPoints.begin(); itr != ControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	ControlPoints.clear();
}

void FBXConverter::ProcessJointsAndAnimations(FbxNode * Node)
{
	FbxMesh* currMesh = Node->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = GetGeometryTransformation(Node);

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			// Update the information in mSkeleton 
			Skeleton.Joints[currJointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			Skeleton.Joints[currJointIndex].mNode = currCluster->GetLink();

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.mBlendingWeight = currCluster->GetControlPointWeights()[i];
				ControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currAnim = &Skeleton.Joints[currJointIndex].mAnimation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames30); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames30);
				*currAnim = new Keyframe();
				(*currAnim)->mFrameNum = i;
				FbxAMatrix currentTransformOffset = Node->EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->mNext);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.mBlendingIndex = 0;
	currBlendingIndexWeightPair.mBlendingWeight = 0;
	for (auto itr = ControlPoints.begin(); itr != ControlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->second->mBlendingInfo.size(); i <= 4; ++i)
		{
			itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

void FBXConverter::ReadNormal(FbxMesh * Mesh, int CtrlPointIndex, int VertexCounter, Vector3 & Normal)
{
	if (Mesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = Mesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIndex).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIndex).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(CtrlPointIndex);
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(VertexCounter);
			Normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			Normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			Normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FBXConverter::ReadTangent(FbxMesh * Mesh, int CtrlPointIndex, int VertexCounter, Vector3 & Tangent)
{
	if (Mesh->GetElementTangentCount() < 1)
	{
		throw std::exception("Invalid Tangent Number");
	}

	FbxGeometryElementTangent* vertexTangent = Mesh->GetElementTangent(0);
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Tangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIndex).mData[0]);
			Tangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIndex).mData[1]);
			Tangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(CtrlPointIndex);
			Tangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			Tangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			Tangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Tangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[0]);
			Tangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[1]);
			Tangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(VertexCounter);
			Tangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			Tangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			Tangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FBXConverter::ReadUV(FbxMesh * Mesh, int CtrlPointIndex, int TextureUVIndex, int UVLayer, Vector2 & UV)
{
	if (UVLayer >= 2 || Mesh->GetElementUVCount() <= UVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = Mesh->GetElementUV(UVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(CtrlPointIndex).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(CtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(CtrlPointIndex);
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(TextureUVIndex).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(TextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

UINT FBXConverter::FindJointIndexUsingName(const string & inJointName)
{
	for (unsigned int i = 0; i < Skeleton.Joints.size(); ++i)
	{
		if (Skeleton.Joints[i].mName == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

