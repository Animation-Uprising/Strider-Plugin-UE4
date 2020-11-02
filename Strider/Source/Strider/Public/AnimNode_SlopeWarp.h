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
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "AnimNode_SlopeWarp.generated.h"


/** A runtime animation node for warping character's legs to conform to a slope */
USTRUCT(BlueprintInternalUseOnly)
struct STRIDER_API FAnimNode_SlopeWarp : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	/** The normal vector representing the slope that the character is currently on. This should
	be determined in your game logic (e.g. EventGraph) */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector SlopeNormal;

	/** A point in space that the plane intersects (usually this is the point 
	where you character character capsule touches the ground */
	UPROPERTY(EditAnywhere, Category = Inputs, meta = (PinHiddenByDefault))
	FVector SlopePoint;

	/** The method used to detect the slope (the slope normal and slope point) */
	UPROPERTY(EditAnywhere, Category = Settings)
	ESlopeDetectionMode SlopeDetectionMode;

	/** How the slope warping should compensate for slope roll which can cause the character's lower body to
	lean into the slope and hips to be off center from the feet. */
	UPROPERTY(EditAnywhere, Category = Settings)
	ESlopeRollCompensation SlopeRollCompensation;

	/** The left vector of the IKRoot relative (Usually the X-Axis if setup with UE4 standard bone orientations).*/
	UPROPERTY(EditAnywhere, Category = Settings)
	FVector IKRootLeftVector;

	/** The maximum slope angle in degrees that slope warping will operate on */
	UPROPERTY(EditAnywhere, Category = Settings)
	float MaxSlopeAngle;

	/** A static offset to apply to the IkRoot. Use this if the are clipping the ground,
	or slightly off it*/
	UPROPERTY(EditAnywhere, Category = Settings)
	float HeightOffset;

	/** How quickly the stride should respond to slope changes. A negative value will default to no smoothing*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = -1.0f))
	float SlopeSmoothingRate;

	/** By default, the slope warp node will only ever compress legs and never extend them. This percentage (0.0 - 1.0) allows for 
	leg extension up to but not beyond a straight leg. It is the percentage of the remaining available leg extension.*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AllowExtensionPercent;

	/** The distance to push the hips down slope relative to the horizontal component of the slope normal.
	Steeper slopes will result in the hips being shifted down hill further. */
	UPROPERTY(EditAnywhere, Category = Settings, meta = (ClampMin = 0.0f))
	float DownSlopeShiftRate;

	/** Reference to the IK Root bone */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FBoneReference IkRoot;

	/** Data defining the 'hips' or 'pelvis' and adjustment parameters */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	FHipAdjustment HipAdjustment;

	/** List of limbs to calculate and apply slope warping to. */
	UPROPERTY(EditAnywhere, Category = BoneReference)
	TArray<FLimbDefinition> Limbs; 

	/** List of additional bones to adjust along with the hips (often used for the IkRoot)*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = BoneReference)
	TArray<FBoneReference> AdditionalBonesToAdjustWithHips;

private:
	FVector LastHipShift;
	FVector CurrentSlopeNormal;
	FVector CurrentSlopePoint;
	FQuat IKRootOffset;
	float DeltaTime;
	bool bValidCheckResult;

	ACharacter* Character;
	UCharacterMovementComponent* CharMoveComponent;
	FAnimInstanceProxy* AnimInstanceProxy;

public:
	FAnimNode_SlopeWarp();

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