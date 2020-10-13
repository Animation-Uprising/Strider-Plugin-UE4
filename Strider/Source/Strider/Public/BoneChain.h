// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "EStriderEnumerations.h"
#include "BoneChain.generated.h"

/** A data structure representing a single bone in a bone chain */
USTRUCT(BlueprintType)
struct FBoneChainLink
{
	GENERATED_BODY()

public:
	/** Reference to the bone which is part of this chain link */
	UPROPERTY(EditAnywhere, Category = Settings)
	FBoneReference Bone;

	/** The weight of this chain link */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f, ClampMax = 1.0f))
	float Weight;

	float NormalizedWeight;

public:
	FBoneChainLink();
};

/** A data structure representing a bone chain in a local space pose */
USTRUCT(BlueprintType)
struct FBoneChain
{
	GENERATED_BODY()

public:
	/** A list defining a bone chain */
	UPROPERTY(EditAnywhere, Category = BoneReferences)
	TArray<FBoneChainLink> BoneChain;

	TArray<FCompactPoseBoneIndex> RootToAnchorBoneIndexHierarchy;

public:
	FBoneChain();
	void Initialize(const FBoneContainer& BoneContainer);
	bool ValidateData(const FBoneContainer& BoneContainer);

	void CalculateAnchorRotation(FQuat& OutAnchorRotation_CS, const FCompactPose& CompactPose);
	void ApplyComponentSpaceRotation(FCompactPose& CompactPose, FQuat& AnchorRotation_CS,
		FVector Axis, float Angle/*Radians*/, float alpha = 1.0f);

private:
	void InitializeBoneChain(const FBoneContainer& BoneContainer);
	void InitializeRootToAnchorHierarchy(const FBoneContainer& BoneContainer);
};