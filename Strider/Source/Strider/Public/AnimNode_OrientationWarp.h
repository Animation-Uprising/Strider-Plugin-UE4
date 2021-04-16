// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "Animation/AnimNodeBase.h"
#include "BoneContainer.h"
#include "BoneChain.h"
#include "AnimNode_OrientationWarp.generated.h"

/** A runtime animation node for warping a characters orientation */
USTRUCT(BlueprintInternalUseOnly)
struct STRIDER_API FAnimNode_OrientationWarp : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Pose)
	FPoseLink InputPose;

	/** The direction (in degrees) to re-orient the character */
	UPROPERTY(EditAnywhere, Category = Input, meta = (PinShownByDefault, ClampMin = -180.0f, ClampMax = 180.0f))
	float Direction;

	/** An offset for the direction. Use this when orientation warping animations that are already not
	facing the way they are going. For example, a strafe right animation should be OrientationWarped with
	an 'Offset' of -90 degrees */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault, ClampMin = -180.0f, ClampMax = 180.0f))
	float Offset;

	/** How much influence the direction should have on the upper body rotation (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault, ClampMin = 0.0f, ClampMax = 1.0f))
	float UpperBodyAlpha;

	/** The axis to be used as Up for the rig (Hint: May be useful if you use 3D modelling software that has Y-axis up. */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector UpAxis;

	/** How much influence this node should have to the overall pose (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AlwaysAsPin, ClampMin = 0.0f, ClampMax = 1.0f))
	float Alpha;

	/** The maximum angle of orientation warping that this node can perform based on the offset. If the 
	direction input is outside of this range, then the direction won't be updated. This 
	is to prevent jitters when suddenly changing directions. */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -180.0f, ClampMax = 180.0f))
	float MaxWarpDelta;

	/** How smoothly the character should rotate to conform to direction changes (degrees / second) 
	A negative value will default to no smoothing*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float Smoothing;

	/** The root bone of your character armature. This will be rotated by the direction value
	to warp the characters orientation */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneReference RootBone;

	/** A chain of animation bones defining the spine. This chain will be progressively
	rotated counter to the warp direction based on their individual weights, so that the 
	character still faces forward*/
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneChain SpineChain;

	/** List of additional bones to adjust with the torso twist. This is intended only for IkRoot type bones
	which share the same orientation as the root bone. For example, rotating the IKHandRoot in opposition of 
	the root rotation to ensure upper body IK nodes stay in line with the rotated upper torso.*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = BoneReference)
	TArray<FBoneReference> RootBonesToCounterAdjust;

private:
	float CurrentDirection;
	bool bValidCheckResult;

public:
	FAnimNode_OrientationWarp();

private:
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