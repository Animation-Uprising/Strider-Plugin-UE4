// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "Animation/AnimNodeBase.h"
#include "BoneContainer.h"
#include "BoneChain.h"
#include "AnimNode_BankWarp.generated.h"

/** A runtime animation node for warping a character's pose based on banking input */
USTRUCT(BlueprintInternalUseOnly)
struct STRIDER_API FAnimNode_BankWarp : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Pose)
	FPoseLink InputPose;

	/** The amount of banking (turning) the character is currently performing.
	Left is -ve and Right is +ve */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinShownByDefault))
	float BankValue;

	/** The alpha (or weight) of this node */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (AlwaysAsPin, ClampMin = 0.0f, ClampMax = 1.0f))
	float Alpha;

	/** The axis to be used as Up for the rig (Hint: May be useful if you use 3D modeling software that has Y-axis up. */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector UpAxis;

	/** The axis to be used as left for the rig (Hint: May be useful if you use 3D modeling software that has X-axis Left. */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector ForwardAxis;

	/** The rate of upper body twisting (in degrees / UnitOfBankValue) */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float TwistRate;

	/** The maximum amount of upper body twisting (in degrees) */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float MaxTwist;

	/** The rate of body leaning (in degrees / UnitOfBankValue) */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float LeanRate;

	/** The maximum amount of body leaning permitted (in degrees) */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float MaxLean;

	/** How smoothly the character should twist and lean to conform to Bank Value changes (units / second) 
	A negative value will default to no smoothing */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float Smoothing;

	/** A reference to the character root bone. This will be used to lean the body */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneReference RootBone;

	/** A chain of weighted bones representing the spine of the character (spine-01 to head) */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneChain SpineChain;

	/** List of additional bones to adjust with the torso twist. This is intended only for IkRoot type bones
	which share the same orientation as the root bone. For example, rotating the IKHandRoot with the  
	torso twist to ensure that the IK Root stays in line with the twisted upper body.*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = BoneReference)
	TArray<FBoneReference> RootBonesToAdjust;

private:
	float CurrentBankValue;
	bool bValidCheckResult;

public:
	FAnimNode_BankWarp();
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
