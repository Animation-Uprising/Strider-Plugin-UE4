// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "Animation/AnimNodeBase.h"
#include "BoneContainer.h"
#include "BoneChain.h"
#include "AnimNode_AccelerationWarp.generated.h"

/** A runtime animation node for warping a character's upper body based on acceleration */
USTRUCT(BlueprintInternalUseOnly)
struct STRIDER_API FAnimNode_AccelerationWarp : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Pose)
	FPoseLink InputPose;

	/** The current acceleration magnitude to warp with (can be negative for deceleration) */
	UPROPERTY(EditAnywhere, Category = Input, meta = (PinShownByDefault))
	float Acceleration;

	/** The direction of acceleration in degrees around the Z-axis (Up-Axis) */
	UPROPERTY(EditAnywhere, Category = Input, meta = (PinHiddenByDefault))
	float Direction;

	/** How much influence this node should have to the overall pose (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AlwaysAsPin, ClampMin = 0.0f, ClampMax = 1.0f))
	float Alpha;

	/** The axis to be used as Up for the rig (Hint: May be useful if you use 3D modeling software that has Y-axis up. */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector UpAxis;

	/** The amount of bending (in degrees) that the torso should perform per unit of 'Acceleration' */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float TorsoBendRatio;

	/** The maximum amount of bending (in degrees) that the torso can perform */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float MaxTorsoBend;

	/** How smoothly the character should lean to conform to acceleration changes (ms^-2 / second) 
	A negative value will default to no smoothing */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float Smoothing;

	/** A bone chain representing the spine of the character. */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneChain SpineChain;

private:
	float CurrentAcceleration;
	bool bValidCheckResult;

public:
	FAnimNode_AccelerationWarp();
	bool ValidateData(const FBoneContainer& BoneContainer);

public:
	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext & Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface
};