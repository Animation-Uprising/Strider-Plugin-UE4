// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"
#include "EStriderEnumerations.h"
#include "StriderData.generated.h"

/** The data structure for a stride warp pivot */
USTRUCT(BlueprintType)
struct FStridePivot
{
	GENERATED_BODY()

public:
	/** The bone used to define the position of the stride pivot */
	UPROPERTY(EditAnywhere, Category = Settings)
	FBoneReference Root;

	/** Should the stride pivot be projected to the ground (set true if using hips) */
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bProjectToGround;

	/** The offset of the stride pivot (along the character forward axis) from the reference bone */
	UPROPERTY(EditAnywhere, Category = Settings)
	float Offset;

	/** The method of setting the stride scale. In most cases 'Manual' is the best option */
	UPROPERTY(EditAnywhere, Category = Settings)
	EStrideVectorMethod StrideVectorMethod;

	/** How smoothly the stride pivot should rotate to conform to direction changes (degrees / second) 
	A negative value will default to no smoothing. */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float Smoothing; 

	/** Set true if not using an offset. The stride pivot will rotate to the nearest axis to the specified direction */
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bChooseNearestAxis;

public:
	float CurrentDirection;
	FTransform Transform;

public:
	FStridePivot();
	void Initialize(const FBoneContainer& Bones);
	bool IsValid(const FBoneContainer& Bones);
	FQuat GetDirectionRotation(float Direction, const float DeltaTime);
};

/** The data structure for a stride warp hip adjustment */
USTRUCT(BlueprintType)
struct FHipAdjustment
{
	GENERATED_BODY()

public:
	/** Reference to the 'hip' or 'pelvis' bone of your character */
	UPROPERTY(EditAnywhere, Category = Settings)
	FBoneReference Hips;

	/** The ratio of calculated hip movement that will be applied between 0 and 1. 0 means no adjustment, 1 means 100% adjustment */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AdjustmentRatio;

	/** The maximum distance the hips can move to recover from being pulled down. 
	A negative value will default to no smoothing*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float MaxRecoveryRate;

public:
	FHipAdjustment();
	void Initialize(const FBoneContainer& Bones);
	bool IsValid(const FBoneContainer& Bones);
};

/** The data structure to define a TwoBone IK Limb for stride warping */
USTRUCT(BlueprintType)
struct FLimbDefinition
{
	GENERATED_BODY()

public:
	/** Reference to the tip bone of your limb (e.g. foot bone) */
	UPROPERTY(EditAnywhere, Category = LimbSettings)
	FBoneReference Tip;

	/** Reference to the IkTarget bone (e.g. ikFoot). Only required for 'External' or 'Both' Ik method */
	UPROPERTY(EditAnywhere, Category = LimbSettings)
	FBoneReference IkTarget;

	/** The number of bones in the IK Chain (usually 2 for a normal human leg */
	UPROPERTY(EditAnywhere, Category = LimbSettings, meta = (ClampMin = 2))
	int32 BoneCount;

public:
	TArray<FBoneReference> Bones;
	float Length;
	float HeightDelta;

	FVector TipLocation_CS;
	FTransform BaseBoneTransform_CS;

public:
	FLimbDefinition();
	void Initialize(const FBoneContainer& PoseBones);
	bool IsValid(const FBoneContainer& PoseBones);
	void CalculateLength(FCSPose<FCompactPose>& Pose);
};