// Copyright 2020 Kenneth Claassen, All Rights Reserved.

#include "AnimNode_StrideWarp.h"
#include "GameFramework/WorldSettings.h"
#include "Animation/AnimInstanceProxy.h"
#include "DrawDebugHelpers.h"
#include "StriderMath.h"

static TAutoConsoleVariable<int32> CVarStrideWarpDebug(
	TEXT("a.AnimNode.Warp.Stride.Debug"),
	0,
	TEXT("Turns stride warp debugging on or off.\n")
	TEXT("<=0: off \n")
	TEXT("  1: minimal (Stride Pivot Only)\n")
	TEXT("  2: full\n"),
	ECVF_SetByConsole);

static TAutoConsoleVariable<int32> CVarStrideWarpEnabled(
	TEXT("a.AnimNode.Warp.Stride.Enable"),
	1,
	TEXT("Turns stride warp nodes on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: on \n"),
	ECVF_SetByConsole);

FAnimNode_StrideWarp::FAnimNode_StrideWarp()
	: StrideScale(1.0f),
	Direction(0.0f),
	Twist(0.0f),
	AllowExtensionPercent(0.0f),
	LastHipShift(0.0f),
	DeltaTime(0.0f),
	bValidCheckResult(false),
	AnimInstanceProxy(nullptr)
{
}

void FAnimNode_StrideWarp::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output,
	TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	EvaluateStride(Output, OutBoneTransforms);
	OutBoneTransforms.Sort(FCompareBoneTransformIndex());

	if (Alpha < 1.0f)
		Output.Pose.LocalBlendCSBoneTransforms(OutBoneTransforms, Alpha);
}

void FAnimNode_StrideWarp::EvaluateStride(FComponentSpacePoseContext& Output, 
	TArray<FBoneTransform>& OutBoneTransforms)
{
	//*******************************************************************************
	//***** Stage 1: Calculate and set the stride pivot in the correct location *****
	//*******************************************************************************
	const FTransform StrideRootTransform_CS = Output.Pose.GetComponentSpaceTransform(StridePivot.Root.CachedCompactPoseIndex);
	FTransform HipTransform_CS = Output.Pose.GetComponentSpaceTransform(HipAdjustment.Hips.CachedCompactPoseIndex);

	StridePivot.Transform.SetRotation(StridePivot.GetDirectionRotation(Direction, DeltaTime));
	if (StridePivot.bProjectToGround)
	{
		FVector rootLocation = StrideRootTransform_CS.GetLocation();
		rootLocation.Z = 0.0f;

		StridePivot.Transform.SetLocation(rootLocation);
	}
	else
	{
		StridePivot.Transform.SetLocation(StrideRootTransform_CS.GetLocation());
	}

	StridePivot.Transform.AddToTranslation(StridePivot.Transform.TransformPosition(FVector(0.0f, StridePivot.Offset, 0.0f)));

	FTransform TwistPivotTransform = StridePivot.Transform;
	TwistPivotTransform.ConcatenateRotation(FQuat(FVector::UpVector, FMath::DegreesToRadians(Twist)));


#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	const USkeletalMeshComponent* SkelMeshComp = AnimInstanceProxy->GetSkelMeshComponent();
	int32 DebugLevel = CVarStrideWarpDebug.GetValueOnAnyThread();

	if (DebugLevel > 0)
	{
		//Draw Stride Pivot
		FTransform ComponentTransform = AnimInstanceProxy->GetComponentTransform();
		AnimInstanceProxy->AnimDrawDebugCoordinateSystem(ComponentTransform.TransformPosition(StridePivot.Transform.GetLocation()),
			ComponentTransform.Rotator() + TwistPivotTransform.Rotator(), 25.0f * StrideScale, false, -1.0f, 2.0f);

		if (DebugLevel > 1)
		{
			//Draw original hips location
			AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(HipTransform_CS.GetLocation(), AnimInstanceProxy),
				10.0f, 15, FColor::Blue, false, -1.0f, 0.25f);
		}
	}
#endif

	//************************************************************************************
	//***** Stage 2: Modify each Limb according to the stride pivot and stride scale *****
	//************************************************************************************
	float HighestTipZDelta = - 1000000.0f;
	for (FLimbDefinition & Limb : Limbs)
	{
		if (Limb.Bones.Num() < 2)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stride Warp Anim Node: Limb Definition has invalid bone setup"))
			continue;
		}

		//Get Relevant transforms, rotations and locations
		Limb.BaseBoneTransform_CS = Output.Pose.GetComponentSpaceTransform(Limb.Bones.Last().CachedCompactPoseIndex);
		
		FVector BaseBoneLocation_CS = Limb.BaseBoneTransform_CS.GetLocation();
		FVector TipLocation_CS = Output.Pose.GetComponentSpaceTransform(Limb.Tip.CachedCompactPoseIndex).GetLocation();

		//Limb Length
		if (Limb.Length < 0.0f)
			Limb.CalculateLength(Output.Pose);

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
		if (DebugLevel > 1)
		{
			//Draw original foot and IK chain
			FVector StartPos = UStriderMath::GetBoneWorldLocation(TipLocation_CS, AnimInstanceProxy);
			FVector EndPos = UStriderMath::GetBoneWorldLocation(BaseBoneLocation_CS, AnimInstanceProxy);

			AnimInstanceProxy->AnimDrawDebugSphere(StartPos, 10.0f, 15, FColor::Blue, false, -1.0f, 0.0f);
			AnimInstanceProxy->AnimDrawDebugLine(StartPos, EndPos, FColor::Blue, false, -1.0f, 0.0f);
		}
#endif
		
		//Scale foot position Y component value in the StridePivotSpace (SPS)
		float MaxLimbLength = (TipLocation_CS - BaseBoneLocation_CS).Size();
		MaxLimbLength += AllowExtensionPercent * (Limb.Length - MaxLimbLength);

		FVector NewTipLocation_SPS = StridePivot.Transform.InverseTransformPosition(TipLocation_CS);
		NewTipLocation_SPS.Y *= StrideScale;

		Limb.TipLocation_CS = TwistPivotTransform.TransformPosition(NewTipLocation_SPS);

		//Push foot back towards hips to ensure legs are only ever compressed and not ever extended
		FVector newBaseToTipVector_CS = Limb.TipLocation_CS - BaseBoneLocation_CS;
		float CurrentLimbLength = newBaseToTipVector_CS.Size();
		if (CurrentLimbLength > MaxLimbLength)
		{
			Limb.TipLocation_CS -= newBaseToTipVector_CS.GetSafeNormal() * (CurrentLimbLength - MaxLimbLength);
		}

		//Check if this Limb has the lowest Z delta
		Limb.HeightDelta = Limb.TipLocation_CS.Z - TipLocation_CS.Z;

		if (Limb.HeightDelta > HighestTipZDelta)
			HighestTipZDelta = Limb.HeightDelta;
	}

	//*****************************************************************************************************
	//***** Stage 3: Pull the hips down so that the character isn't floating *****
	//*****************************************************************************************************
	// Clamp HipAdjust upon recovery
	float HipShift = -HighestTipZDelta * HipAdjustment.AdjustmentRatio;
	if (HipAdjustment.MaxRecoveryRate > 0.0f && HipShift > LastHipShift)
		LastHipShift = UStriderMath::MoveToward(LastHipShift, HipShift, HipAdjustment.MaxRecoveryRate * DeltaTime);
	else
		LastHipShift = HipShift;
	
	const FVector HipAdjust = FVector(0.0f, 0.0f, LastHipShift);
	const FVector CurrentHipLocation_CS = HipTransform_CS.GetLocation();

	FVector NewHipLocation_CS = CurrentHipLocation_CS + HipAdjust;

	HipTransform_CS.SetLocation(NewHipLocation_CS);
	OutBoneTransforms.Add(FBoneTransform(HipAdjustment.Hips.CachedCompactPoseIndex, HipTransform_CS));

	//Adjust any additional bones set
	for (FBoneReference& Bone : AdditionalBonesToAdjustWithHips)
	{
		FTransform BoneTransform_CS = Output.Pose.GetComponentSpaceTransform(Bone.CachedCompactPoseIndex);

		BoneTransform_CS.SetLocation(BoneTransform_CS.GetLocation() + HipAdjust);
		OutBoneTransforms.Add(FBoneTransform(Bone.CachedCompactPoseIndex, BoneTransform_CS));
	}

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	if (DebugLevel > 1)
	{
		//Draw final hips location
		FVector HipLocation_GS = UStriderMath::GetBoneWorldLocation(HipTransform_CS, AnimInstanceProxy);
		AnimInstanceProxy->AnimDrawDebugSphere(HipLocation_GS, 10.0f, 15, FColor::Green, false, -1.0f, 0.0f);
	}
#endif

	//******************************************************************
	//***** Stage 4: Apply the hip adjustment to the limbs as well *****
	//******************************************************************
	for (int32 i = 0; i < Limbs.Num(); ++i)
	{
		FLimbDefinition& Limb = Limbs[i];

		FVector FootAdjust = HipAdjust + FVector(0.0f, 0.0f, ((HighestTipZDelta)-Limb.HeightDelta) * HipAdjustment.AdjustmentRatio + HipShift - LastHipShift);
		Limb.TipLocation_CS += FootAdjust;

		FTransform IkTransform_CS = Output.Pose.GetComponentSpaceTransform(Limb.Tip.CachedCompactPoseIndex);
		IkTransform_CS.SetLocation(Limb.TipLocation_CS);

		OutBoneTransforms.Add(FBoneTransform(Limb.IkTarget.CachedCompactPoseIndex, IkTransform_CS));

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
		if (DebugLevel > 1)
		{
			Limb.BaseBoneTransform_CS.AddToTranslation(HipAdjust);

			//Draw final foot and IK chain
			FVector StartPos = UStriderMath::GetBoneWorldLocation(Limb.TipLocation_CS + HipAdjust, AnimInstanceProxy);
			FVector EndPos = UStriderMath::GetBoneWorldLocation(Limb.BaseBoneTransform_CS.GetLocation(), AnimInstanceProxy);

			AnimInstanceProxy->AnimDrawDebugSphere(StartPos, 10.0f, 15, FColor::Green, false, -1.0f, 0.0f);
			AnimInstanceProxy->AnimDrawDebugLine(StartPos, EndPos, FColor::Green, false, -1.0f, 0.0f);
		}
#endif
	}
}

bool FAnimNode_StrideWarp::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return CheckValidBones(RequiredBones)
		   && Alpha > 0.00001f
		   && (CVarStrideWarpEnabled.GetValueOnAnyThread() == 1)
		   && IsLODEnabled(AnimInstanceProxy);
}

void FAnimNode_StrideWarp::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
	AnimInstanceProxy = Context.AnimInstanceProxy;
}

void FAnimNode_StrideWarp::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);
}

void FAnimNode_StrideWarp::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);
	DeltaTime = Context.GetDeltaTime();
}

void FAnimNode_StrideWarp::GatherDebugData(FNodeDebugData& DebugData)
{
	ComponentPose.GatherDebugData(DebugData);
}

bool FAnimNode_StrideWarp::HasPreUpdate() const
{
	return StridePivot.StrideVectorMethod == EStrideVectorMethod::ActorVelocity;
}

void FAnimNode_StrideWarp::PreUpdate(const UAnimInstance* InAnimInstance)
{
	const USkeletalMeshComponent* SkelMesh = InAnimInstance->GetSkelMeshComponent();
	AActor* Actor = SkelMesh->GetOwner();

	if (Actor)
	{
		FVector localVelocity = Actor->GetActorTransform().TransformVector(Actor->GetVelocity());
		localVelocity.Z = 0.0f;
		Direction = localVelocity.GetSafeNormal().HeadingAngle();
	}
}

void FAnimNode_StrideWarp::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	HipAdjustment.Initialize(RequiredBones);
	StridePivot.Initialize(RequiredBones);

	bool bStrideWarpSetupValid = true; 
	if (Limbs.Num() > 0)
	{
		for (FLimbDefinition& LimbDef : Limbs)
		{
			LimbDef.Initialize(RequiredBones);

			if (!LimbDef.IsValid(RequiredBones))
			{
				UE_LOG(LogTemp, Warning, TEXT("Stride Warp Anim Node: Invalid limb setup, node will not take effect."))
				bStrideWarpSetupValid = false;
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stride Warp Anim Node: No limb definitions, node will not take effect."))
		bStrideWarpSetupValid = false;
	}

	for (FBoneReference& Bone : AdditionalBonesToAdjustWithHips)
	{
		Bone.Initialize(RequiredBones);

		if (!Bone.IsValidToEvaluate(RequiredBones))
		{
			UE_LOG(LogTemp, Warning, TEXT("Slope Warp Anim NOde: Invalid 'additional bone' found in setup, this bone will not be affected,"))
		}
	}

	if (!HipAdjustment.IsValid(RequiredBones))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stride Warp Anim Node: Hip/Pelvis setup is invalid, node will not take effect."))
		bStrideWarpSetupValid = false;
	}

	if(!StridePivot.IsValid(RequiredBones))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stride Warp Anim Node: Stride Pivot setup is invalid, node will not take effect."))
		bStrideWarpSetupValid = false;
	}

	bValidCheckResult = bStrideWarpSetupValid;
}

bool FAnimNode_StrideWarp::CheckValidBones(const FBoneContainer& RequiredBones)
{
	bValidCheckResult = HipAdjustment.IsValid(RequiredBones)
		&& StridePivot.IsValid(RequiredBones)
		&& (Limbs.Num() > 0);

	for (FLimbDefinition& LimbDef : Limbs)
	{
		if (!LimbDef.IsValid(RequiredBones))
		{
			bValidCheckResult = false;
			break;
		}
	}

	for (FBoneReference& Bone : AdditionalBonesToAdjustWithHips)
	{
		if (!Bone.IsValidToEvaluate(RequiredBones))
		{
			bValidCheckResult = false;
			break;
		}
	}

	return bValidCheckResult;
}