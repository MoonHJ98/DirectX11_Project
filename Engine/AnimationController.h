#pragma once

#include "Includes.h"

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

public:
	void	Initialize(const aiScene* pScene);
	bool	SetAnimIndex(UINT pAnimIndex);// this takes an index to set the current animation to
	bool	SetAnimation(const string& name);// this takes a string to set the animation to, i.e. SetAnimation("Idle");
	int		GetBoneIndex(const string& bname);

	vector<XMFLOAT4X4>& GetTransforms(float _FrameTime) { return Animations[CurrentAnimIndex].GetTransforms(_FrameTime); }


	void	Release();
protected:

	void	UpdateTransforms(BONE* pNode);
	void	CalculateBoneToWorldTransform(BONE* pInternalNode);/** Calculates the global transformation matrix for the given internal node */

	void	Calculate(float pTime);
	void	CalcBoneMatrices();


	void	ExtractAnimations(const aiScene* pScene);
	BONE*	CreateBoneTree(aiNode* pNode, BONE* pParent);// Recursively creates an internal node structure matching the current scene and animation.

public:
	vector<AnimEvaluator> Animations;// a std::vector that holds each animation 
	int32_t CurrentAnimIndex;/** Current animation index */

protected:
	BONE* Skeleton;/** Root node of the internal scene structure */
	map<string, BONE*> BonesByName;/** Name to node map to quickly find nodes by their name */
	map<string, UINT> BonesToIndex;/** Name to node map to quickly find nodes by their name */
	map<string, uint32_t> AnimationNameToId;// find animations quickly
	vector<BONE*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
	vector<XMFLOAT4X4> Transforms;// temp array of transfrorms
 
};

