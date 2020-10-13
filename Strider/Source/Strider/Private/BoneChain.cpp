// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "BoneChain.h"

FBoneChainLink::FBoneChainLink()
	: Weight(1.0f), NormalizedWeight(1.0f)
{
}

FBoneChain::FBoneChain()
{
}

void FBoneChain::Initialize(const FBoneContainer& BoneContainer)
{
	InitializeBoneChain(BoneContainer);
	ValidateData(BoneContainer);
	InitializeRootToAnchorHierarchy(BoneContainer);
}

bool FBoneChain::ValidateData(const FBoneContainer& BoneContainer)
{
	if (BoneChain.Num() == 0)
		return false;

	for (FBoneChainLink & BoneLink : BoneChain)
	{
		if (!BoneLink.Bone.IsValidToEvaluate(BoneContainer))
			return false;
	}

	return true;
}

void FBoneChain::CalculateAnchorRotation(FQuat& OutAnchorRotation_CS, const FCompactPose& CompactPose)
{
	//Accumulate rotations until the anchor is reached to find the Anchor 'component space' transform
	//This avoids having to convert the entire pose to component space
	for (FCompactPoseBoneIndex & BoneIndex : RootToAnchorBoneIndexHierarchy)
	{
		OutAnchorRotation_CS = OutAnchorRotation_CS * CompactPose[BoneIndex].GetRotation();
	}
}

void FBoneChain::ApplyComponentSpaceRotation(FCompactPose& CompactPose, FQuat& AnchorRotation_CS,
	FVector Axis, float Angle /*Radians*/, float Alpha /*= 1.0f*/)
{
	//Iterate through each link bone in the bone chain and apply a proportion of the rotation
	for (FBoneChainLink & BoneLink : BoneChain)
	{
		//Get the local space transform of this link
		FTransform& LinkTransform_Local = CompactPose[BoneLink.Bone.CachedCompactPoseIndex];
		AnchorRotation_CS = AnchorRotation_CS * LinkTransform_Local.GetRotation();

		//Get the weighted link rotation and in component space and convert it to local space of the links component space transform
		FQuat LinkRotation_CS = FQuat(Axis, Angle * BoneLink.NormalizedWeight * Alpha);
		FQuat LinkRotation_Local = AnchorRotation_CS.Inverse() * LinkRotation_CS * AnchorRotation_CS;

		//Apply the local rotation
		LinkTransform_Local.ConcatenateRotation(LinkRotation_Local);
		//LinkTransform_Local.SetRotation(LinkTransform_Local.GetRotation() * LinkRotation_Local);
		LinkTransform_Local.NormalizeRotation();
	}
}

void FBoneChain::InitializeBoneChain(const FBoneContainer& BoneContainer)
{
	float AccumulatedNegativeWeight = 0.0f;
	float AccumulatedPositiveWeight = 0.0f;
	for (FBoneChainLink& BoneLink : BoneChain)
	{
		BoneLink.Bone.Initialize(BoneContainer);

		if (BoneLink.Weight > 0.00001f)
		{
			AccumulatedPositiveWeight += BoneLink.Weight;
		}
		else if (BoneLink.Weight < -0.00001f)
		{
			AccumulatedNegativeWeight += FMath::Abs(BoneLink.Weight);
		}
	}

	for (FBoneChainLink& BoneLink : BoneChain)
	{
		if (BoneLink.Weight > 0.00001f)
		{
			BoneLink.NormalizedWeight = BoneLink.Weight * (1.0f / AccumulatedPositiveWeight);
		}
		else if (BoneLink.Weight < -0.00001f)
		{
			BoneLink.NormalizedWeight = BoneLink.Weight * (1.0f / AccumulatedNegativeWeight);
		}
		else
		{
			BoneLink.NormalizedWeight = 0.0f;
		}
	}
}

void FBoneChain::InitializeRootToAnchorHierarchy(const FBoneContainer& BoneContainer)
{
	
	if (BoneChain.Num() > 0)
	{
		RootToAnchorBoneIndexHierarchy.Empty(2);

		FCompactPoseBoneIndex BoneIndex = BoneChain[0].Bone.CachedCompactPoseIndex;

		if (BoneIndex > 0)
		{
			FCompactPoseBoneIndex ParentBoneIndex = BoneContainer.GetParentBoneIndex(BoneIndex);
			while (ParentBoneIndex > 0)
			{
				RootToAnchorBoneIndexHierarchy.Insert(ParentBoneIndex, 0);
				ParentBoneIndex = BoneContainer.GetParentBoneIndex(ParentBoneIndex);
			}
		}
	}
}

