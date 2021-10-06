#include "pch.h"
#include "AnimationController.h"

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
	Release();
}

void AnimationController::Initialize(const aiScene * pScene)
{
	if (!pScene->HasAnimations())
		return;

	Release();

	Skeleton = CreateBoneTree(pScene->mRootNode, NULL);
	ExtractAnimations(pScene);

	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];

		for (unsigned int n = 0; n < mesh->mNumBones; ++n) 
		{
			const aiBone* bone = mesh->mBones[n];
			auto found = BonesByName.find(bone->mName.data);
			if (found != BonesByName.end())
			{// FOUND IT!!! woohoo, make sure its not already in the bone list
				bool skip = false;
				for (size_t j(0); j < Bones.size(); j++)
				{
					string bname = bone->mName.data;
					if (Bones[j]->Name == bname) {
						skip = true;// already inserted, skip this so as not to insert the same bone multiple times
						break;
					}
				}
				if (!skip)
				{// only insert the bone if it has not already been inserted
					string tes = found->second->Name;
					TransformMatrix(found->second->Offset, bone->mOffsetMatrix);
					XMMATRIX mat = XMLoadFloat4x4(&found->second->Offset);	// [CUSTOM]
					XMStoreFloat4x4(&found->second->Offset, XMMatrixTranspose(mat));
					//found->second->Offset.Transpose();// transpoce their matrix to get in the correct format
					Bones.push_back(found->second);
					BonesToIndex[found->first] = Bones.size() - 1;
				}
			}
		}
	}
	Transforms.resize(Bones.size());
	float timestep = 1.0f / 30.0f;// 30 per second
	for (size_t i = 0; i < Animations.size(); i++)
	{// pre calculate the animations
		SetAnimIndex(i);
		float dt = 0;
		for (float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond / 30.0f)
		{
			dt += timestep;
			Calculate(dt);
			Animations[i].Transforms.push_back(vector<XMFLOAT4X4>());
			vector<XMFLOAT4X4>& trans = Animations[i].Transforms.back();
			for (size_t a = 0; a < Transforms.size(); ++a)
			{
				XMMATRIX result = XMLoadFloat4x4(&Bones[a]->Offset) * XMLoadFloat4x4(&Bones[a]->GlobalTransform);
				XMFLOAT4X4 rotationmat;
				XMStoreFloat4x4(&rotationmat, result); // [CUSTOM]

				trans.push_back(rotationmat);
			}
		}
	}

}

bool AnimationController::SetAnimIndex(UINT pAnimIndex)
{
	if (pAnimIndex >= Animations.size())
		return false;// no change, or the animations data is out of bounds
	int32_t oldindex = CurrentAnimIndex;
	CurrentAnimIndex = pAnimIndex;// only set this after the checks for good data and the object was actually inserted

	return oldindex != CurrentAnimIndex;
}

bool AnimationController::SetAnimation(const string & name)
{
	map<string, uint32_t>::iterator itr = AnimationNameToId.find(name);
	int32_t oldindex = CurrentAnimIndex;
	if (itr != AnimationNameToId.end())
		CurrentAnimIndex = itr->second;

	return oldindex != CurrentAnimIndex;
}

void AnimationController::Release()
{
	CurrentAnimIndex = -1;
	Animations.clear();// clear all animations
	delete Skeleton;// This node will delete all children recursivly
	Skeleton = nullptr;// make sure to zero it out
}

void AnimationController::UpdateTransforms(BONE * pNode)
{
	CalculateBoneToWorldTransform(pNode);// update global transform as well
	for (auto it = pNode->Children.begin(); it != pNode->Children.end(); ++it)// continue for all children
		UpdateTransforms(*it);
}

void AnimationController::CalculateBoneToWorldTransform(BONE * Child)
{
	Child->GlobalTransform = Child->LocalTransform;
	BONE* parent = Child->Parent;
	while (parent)
	{// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
		XMMATRIX result = XMLoadFloat4x4(&Child->GlobalTransform) * XMLoadFloat4x4(&parent->LocalTransform);
		XMStoreFloat4x4(&Child->GlobalTransform, result); // [CUSTOM]

		parent = parent->Parent;// get the parent of the bone we are working on 
	}
}

void AnimationController::Calculate(float pTime)
{
	if ((CurrentAnimIndex < 0) | (CurrentAnimIndex >= Animations.size())) return;// invalid animation

	Animations[CurrentAnimIndex].Evaluate(pTime, BonesByName);
	UpdateTransforms(Skeleton);
}

void AnimationController::CalcBoneMatrices()
{
	for (size_t a = 0; a < Transforms.size(); ++a)
	{
		XMMATRIX result = XMLoadFloat4x4(&Bones[a]->Offset) * XMLoadFloat4x4(&Bones[a]->GlobalTransform);
		XMStoreFloat4x4(&Transforms[a], result);
	}
}

int AnimationController::GetBoneIndex(const string & bname)
{
	map<string, unsigned int>::iterator found = BonesToIndex.find(bname);
	if (found != BonesToIndex.end())
		return found->second;
	else return -1;
}

void AnimationController::ExtractAnimations(const aiScene * pScene)
{
	for (size_t i(0); i < pScene->mNumAnimations; i++) {
		Animations.push_back(AnimEvaluator(pScene->mAnimations[i]));// add the animations
	}
	for (uint32_t i(0); i < Animations.size(); i++) {// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(map<string, uint32_t>::value_type(Animations[i].Name, i));
	}

	CurrentAnimIndex = 0;
	SetAnimation("Take 001");
}

BONE * AnimationController::CreateBoneTree(aiNode * pNode, BONE * pParent)
{
	BONE* Bone = new BONE();// create a node
	Bone->Name = pNode->mName.data;// get the name of the bone
	Bone->Parent = pParent; //set the parent, in the case this is theroot node, it will be null
	
	BonesByName[Bone->Name] = Bone;// use the name as a key
	TransformMatrix(Bone->LocalTransform, pNode->mTransformation);

	XMMATRIX mat = XMLoadFloat4x4(&Bone->LocalTransform);	// [CUSTOM]
	XMStoreFloat4x4(&Bone->LocalTransform, XMMatrixTranspose(mat));

	Bone->OriginalLocalTransform = Bone->LocalTransform;// a copy saved
	CalculateBoneToWorldTransform(Bone);

	// continue for all child nodes and assign the created internal nodes as our children
	for (unsigned int a = 0; a < pNode->mNumChildren; a++) {// recursivly call this function on all children
		Bone->Children.push_back(CreateBoneTree(pNode->mChildren[a], Bone));
	}

	return Bone;
}


