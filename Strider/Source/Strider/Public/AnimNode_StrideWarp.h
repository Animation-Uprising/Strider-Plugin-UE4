// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "StriderData.h"
#include "EStriderEnumerations.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimNode_StrideWarp.generated.h"

/** A runtime animation node for warping/scaling the stride of characters */
USTRUCT(BlueprintInternalUseOnly)
struct STRIDER_API FAnimNode_StrideWarp : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	/** The amount of scaling to apply to the stride. A value of 1 will produce a normal animation, values < 1 will
	cause the stride to become smaller and values > 1 will cause the stride to become larger. */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault, ClampMin = 0.0f))
	float StrideScale;

	/** The direction (in degrees) to scale the stride in. This could be your blend space or orientation warping direction */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault, ClampMin = -180.0f, ClampMax = 180.0f))
	float Direction;

	/** The amount of twist (in degrees) to apply to the stride.  */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault, ClampMin = -90.0f, ClampMax = 90.0f))
	float Twist;

	/** By default strider will only ever compress legs never extend them. This percentage (0.0 - 1.0) allows for 
	leg extension up to but not beyond a straight leg. It is the percentage of the remaining available leg extension.*/
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AllowExtensionPercent;

	/** Data representing the stride pivot where all stride warping is scaled from*/
	UPROPERTY(EditAnywhere, Category = Bones)
	FStridePivot StridePivot;

	/** Data defining the 'hips' or 'pelvis' and adjustment parameters */
	UPROPERTY(EditAnywhere, Category = Bones)
	FHipAdjustment HipAdjustment;

	/** List of limbs to calculate and apply stride warping to. */
	UPROPERTY(EditAnywhere, Category = Bones)
	TArray<FLimbDefinition> Limbs; 

	/** List of additional bones to adjust along with the hips (often used for the IkRoot)*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Bones)
	TArray<FBoneReference> AdditionalBonesToAdjustWithHips; 

private:
	float LastHipShift;
	float DeltaTime;
	bool bValidCheckResult;

	FAnimInstanceProxy* AnimInstanceProxy;

public:
	FAnimNode_StrideWarp();

private: 
	void EvaluateStride(FComponentSpacePoseContext& Output,
		TArray<FBoneTransform>& OutBoneTransforms);

	bool CheckValidBones(const FBoneContainer& RequiredBones);

public:
	//FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, 
		TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	//End of FAnimNode_SkeletalControlBase interface

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual bool HasPreUpdate() const override;
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	// End of FAnimNode_Base_Interface

private:
	//FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	//End of FAnimNode_SkeletalControlBase interface
};