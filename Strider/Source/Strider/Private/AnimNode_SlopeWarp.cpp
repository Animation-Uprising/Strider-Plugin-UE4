// Copyright 2020 Kenneth Claassen, All Rights Reserved.

#include "AnimNode_SlopeWarp.h"
#include "GameFramework/WorldSettings.h"
#include "Animation/AnimInstanceProxy.h"
#include "DrawDebugHelpers.h"
#include "..\Public\StriderMath.h"

static TAutoConsoleVariable<int32> CVarSlopeWarpDebug(
	TEXT("a.AnimNode.Warp.Slope.Debug"),
	0,
	TEXT("Turns slope warp debugging on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: minimal \n")
	TEXT("  2: draw plane \n")
	TEXT("  3: full \n"),
	ECVF_SetByConsole);

static TAutoConsoleVariable<int32> CVarSlopeWarpEnabled(
	TEXT("a.AnimNode.Warp.Slope.Enable"),
	1,
	TEXT("Turns slope warp nodes on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: on \n"),
	ECVF_SetByConsole);

FAnimNode_SlopeWarp::FAnimNode_SlopeWarp()
	: SlopeNormal(FVector::UpVector),
	SlopePoint(FVector::ZeroVector),
	SlopeDetectionMode(ESlopeDetectionMode::ManualSlope),
	SlopeRollCompensation(ESlopeRollCompensation::AdjustHips),
	IKRootLeftVector(FVector::ForwardVector),
	MaxSlopeAngle(45.0f),
	HeightOffset(0.0f),
	SlopeSmoothingRate(-1.0f),
	AllowExtensionPercent(0.0f),
	DownSlopeShiftRate(20.0f),
	LastHipShift(FVector::ZeroVector),
	CurrentSlopeNormal(FVector::UpVector),
	CurrentSlopePoint(FVector::ZeroVector),
	IKRootOffset(FQuat::Identity),
	DeltaTime(0.0f),
	bValidCheckResult(false),
	Character(nullptr),
	CharMoveComponent(nullptr),
	AnimInstanceProxy(nullptr)
{
}

void FAnimNode_SlopeWarp::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output,
	TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	//Get relevant transforms
	FTransform RootTransform_CS = Output.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(0));
	FTransform HipTransform_CS = Output.Pose.GetComponentSpaceTransform(HipAdjustment.Hips.CachedCompactPoseIndex);
	FTransform IkRootTransform_CS = Output.Pose.GetComponentSpaceTransform(IkRoot.CachedCompactPoseIndex);
	FVector IkRootLocation_CS = IkRootTransform_CS.GetLocation();

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	const USkeletalMeshComponent* SkelMeshComp = AnimInstanceProxy->GetSkelMeshComponent();
	int32 DebugLevel = CVarSlopeWarpDebug.GetValueOnAnyThread();

	if (DebugLevel > 2)
	{
		//Draw original hips and original Ik Root
		AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(IkRootLocation_CS, AnimInstanceProxy), 
			5.0f, 2, FColor::Blue, false, -1.0f, 0.0f);

		AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(HipTransform_CS, AnimInstanceProxy),
			10.0f, 15, FColor::Blue, false, -1.0f, 0.0f);
	}
#endif //ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	
	//Project IkRoot to floor  
	float HeightShift = UStriderMath::GetPointOnPlane(IkRootLocation_CS, CurrentSlopeNormal, CurrentSlopePoint);

	//Shift down Root, IkRoot and Hips so that it is in contact with the floor
	IkRootLocation_CS.Z += HeightShift + HeightOffset;
	FVector HipShift = FVector(0.0f, 0.0f, HeightShift + HeightOffset);
	RootTransform_CS.AddToTranslation(HipShift);

	//Create a slope transform in component space using the slope normal and cross product of component axis
	FVector Forward = CurrentSlopeNormal.CrossProduct(CurrentSlopeNormal, IkRootTransform_CS.TransformVector(IKRootLeftVector));
	FVector Right = CurrentSlopeNormal.CrossProduct(Forward, CurrentSlopeNormal);

	//Shift the hips downhill (relative to character facing direction)
	HipShift -= Forward * DownSlopeShiftRate * Forward.Z;

	IkRootTransform_CS = FTransform(Right, Forward, CurrentSlopeNormal, IkRootLocation_CS);
	IkRootTransform_CS.SetRotation(IkRootTransform_CS.GetRotation() * IKRootOffset);

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	if (DebugLevel > 0)
	{
		//Draw slope axis
		FTransform ComponentTransform = AnimInstanceProxy->GetComponentTransform();
		AnimInstanceProxy->AnimDrawDebugCoordinateSystem(ComponentTransform.TransformPosition(IkRootTransform_CS.GetLocation()), 
			ComponentTransform.Rotator() + IkRootTransform_CS.Rotator(), 20.0f, false, -1.0f, 2.0f);

		if(DebugLevel > 1)
		{
			//Draw slope plane
			FTransform planeTransform = IkRootTransform_CS;
			planeTransform.SetLocation(UStriderMath::GetBoneWorldLocation(planeTransform, AnimInstanceProxy));
			planeTransform.SetRotation(ComponentTransform.TransformRotation(IkRootTransform_CS.GetRotation() * IKRootOffset.Inverse())); // * IKRootOffset);
			AnimInstanceProxy->AnimDrawDebugPlane(planeTransform, 100.0f, FColor::Cyan, false, -1, 0.0f);

			if (DebugLevel > 2)
			{
				//Draw IkRoot after adjustment
				AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(IkRootLocation_CS, AnimInstanceProxy),
					5.0f, 2, FColor::Green, false, -1.0f, 0.0f);

				//Draw hips after downhill shift
				AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(HipTransform_CS.GetLocation() + HipShift, AnimInstanceProxy),
					10.0f, 15, FColor::Yellow, false, -1.0f, 0.0f);
			}
		}
	}
#endif //ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	
	OutBoneTransforms.Add(FBoneTransform(FCompactPoseBoneIndex(0), RootTransform_CS));
	OutBoneTransforms.Add(FBoneTransform(IkRoot.CachedCompactPoseIndex, IkRootTransform_CS));

	FVector AverageHorizontalShift = FVector::ZeroVector;
	for (FLimbDefinition& Limb : Limbs)
	{
		//Limb Length
		if (Limb.Length < 0.0f)
			Limb.CalculateLength(Output.Pose);

		Limb.BaseBoneTransform_CS = Output.Pose.GetComponentSpaceTransform(Limb.Bones.Last().CachedCompactPoseIndex);
		FVector BaseLocation_CS = Limb.BaseBoneTransform_CS.GetLocation();
		FTransform TipTransform_CS = Output.Pose.GetComponentSpaceTransform(Limb.IkTarget.CachedCompactPoseIndex);
		Limb.TipLocation_CS = TipTransform_CS.GetLocation();

		FTransform TipTransform_Local = Output.Pose.GetLocalSpaceTransform(Limb.IkTarget.CachedCompactPoseIndex);

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
		if (DebugLevel > 2)
		{
			//Draw original foot and IK chain
			FVector TipPosition_GL = UStriderMath::GetBoneWorldLocation(Limb.TipLocation_CS, AnimInstanceProxy);
			AnimInstanceProxy->AnimDrawDebugSphere(TipPosition_GL, 10.0f, 15, FColor::Blue, false, -1.0f, 0.0f);
			AnimInstanceProxy->AnimDrawDebugLine(TipPosition_GL, UStriderMath::GetBoneWorldLocation(BaseLocation_CS, AnimInstanceProxy),
				FColor::Blue, false, -1.0f, 0.0f);
		}
#endif //ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG

		//Determine Maximum allowable limb length
		float MaxLimbLength = (Limb.TipLocation_CS - BaseLocation_CS).Size();
		MaxLimbLength += AllowExtensionPercent * (Limb.Length - MaxLimbLength);

		//Determine new foot position
		
		FVector NewTipLocation_CS = Limb.TipLocation_CS;

		if(SlopeRollCompensation == ESlopeRollCompensation::AdjustFeet)
		{
			TipTransform_CS.SetRotation(IkRootTransform_CS.TransformRotation(TipTransform_Local.GetRotation()));
			NewTipLocation_CS.Z += UStriderMath::GetPointOnPlane(NewTipLocation_CS, CurrentSlopeNormal, CurrentSlopePoint);
			TipTransform_CS.SetLocation(NewTipLocation_CS);
			Limb.TipLocation_CS = NewTipLocation_CS;

			OutBoneTransforms.Add(FBoneTransform(Limb.IkTarget.CachedCompactPoseIndex, TipTransform_CS));
		}
		else
		{
			FVector TipLocation_Local = TipTransform_Local.GetLocation();
			NewTipLocation_CS = IkRootTransform_CS.TransformPosition(TipLocation_Local);

			AverageHorizontalShift += NewTipLocation_CS - Limb.TipLocation_CS;
		}

		//Shift hips back towards an over-extended limb
		FVector NewTipToBaseVector = NewTipLocation_CS - (BaseLocation_CS + HipShift);
		float CurrentLength = NewTipToBaseVector.Size();
		if (CurrentLength > MaxLimbLength)
		{
			HipShift += NewTipToBaseVector.GetUnsafeNormal() *	(CurrentLength - MaxLimbLength);
		}
	}

	if(SlopeRollCompensation == ESlopeRollCompensation::AdjustHips)
	{
		AverageHorizontalShift.Z = 0.0f;
		AverageHorizontalShift /= Limbs.Num();
		HipShift += AverageHorizontalShift;
	}

	//Clamp HipShift upon recovery
	if (HipAdjustment.MaxRecoveryRate > 0.0f 
		&& LastHipShift.SizeSquared() - HipShift.SizeSquared() > HipAdjustment.MaxRecoveryRate * DeltaTime)
	{
		UStriderMath::MoveTowardVector(LastHipShift, HipShift, HipAdjustment.MaxRecoveryRate * DeltaTime);
	}
	else
	{
		LastHipShift = HipShift;
	}

	//Apply hip shift to the hip transform
	HipTransform_CS.AddToTranslation(LastHipShift);

	//Adjust any additional bones along with the hips
	for (FBoneReference& Bone : AdditionalBonesToAdjustWithHips)
	{
		FTransform BoneTransform_CS = Output.Pose.GetComponentSpaceTransform(Bone.CachedCompactPoseIndex);

		BoneTransform_CS.SetLocation(BoneTransform_CS.GetLocation() + LastHipShift);
		OutBoneTransforms.Add(FBoneTransform(Bone.CachedCompactPoseIndex, BoneTransform_CS));
	}

#if ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG
	if (DebugLevel > 2)
	{
		//Draw final hip location
		AnimInstanceProxy->AnimDrawDebugSphere(UStriderMath::GetBoneWorldLocation(HipTransform_CS, AnimInstanceProxy),
			10.0f, 15, FColor::Green, false, -1.0f, 0.0f);

		//Draw final foot & IkChain for each limb
		if (SlopeRollCompensation == ESlopeRollCompensation::AdjustFeet)
		{
			for (FLimbDefinition& Limb : Limbs)
			{
				FVector IkTargetLocation_GS = UStriderMath::GetBoneWorldLocation(Limb.TipLocation_CS, AnimInstanceProxy);

				AnimInstanceProxy->AnimDrawDebugSphere(IkTargetLocation_GS, 10.0f, 15, FColor::Green, false, -1.0f, 0.0f);
				FVector BaseLocation_GS = UStriderMath::GetBoneWorldLocation(Limb.BaseBoneTransform_CS.GetLocation() + LastHipShift, AnimInstanceProxy);
				AnimInstanceProxy->AnimDrawDebugLine(IkTargetLocation_GS, BaseLocation_GS, FColor::Green, false, -1.0f, 0.0f);
			}
		}
		else
		{
			for (FLimbDefinition& Limb : Limbs)
			{
				FTransform IkTarget_LS = Output.Pose.GetLocalSpaceTransform(Limb.IkTarget.CachedCompactPoseIndex);
				FVector IkTargetLocation_CS = IkRootTransform_CS.TransformPosition(IkTarget_LS.GetLocation());
				FVector IkTargetLocation_GS = UStriderMath::GetBoneWorldLocation(IkTargetLocation_CS, AnimInstanceProxy);

				AnimInstanceProxy->AnimDrawDebugSphere(IkTargetLocation_GS, 10.0f, 15, FColor::Green, false, -1.0f, 0.0f);

				FVector BaseLocation_GS = UStriderMath::GetBoneWorldLocation(Limb.BaseBoneTransform_CS.GetLocation() + LastHipShift, AnimInstanceProxy);

				AnimInstanceProxy->AnimDrawDebugLine(IkTargetLocation_GS, BaseLocation_GS, FColor::Green, false, -1.0f, 0.0f);
			}
		}
	}
#endif //ENABLE_ANIM_DEBUG && ENABLE_DRAW_DEBUG

	//Output Bone Transforms
	OutBoneTransforms.Add(FBoneTransform(HipAdjustment.Hips.CachedCompactPoseIndex, HipTransform_CS));
	
	OutBoneTransforms.Sort(FCompareBoneTransformIndex());

	//Apply node alpha
	if (Alpha < 1.0f)
		Output.Pose.LocalBlendCSBoneTransforms(OutBoneTransforms, Alpha);
}

bool FAnimNode_SlopeWarp::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return bValidCheckResult
		   && (Alpha > 0.00001f)
		   && (CVarSlopeWarpEnabled.GetValueOnAnyThread() == 1)
		   && IsLODEnabled(AnimInstanceProxy);
}

void FAnimNode_SlopeWarp::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
	AnimInstanceProxy = Context.AnimInstanceProxy;
}

void FAnimNode_SlopeWarp::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);
}

void FAnimNode_SlopeWarp::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);
	DeltaTime = Context.GetDeltaTime();

	//Clamp the slope if it is beyond max slope degrees (Use a fast cheety method)
	SlopeNormal.Normalize();
	float SlopeAngle = FMath::Abs(UStriderMath::AngleBetween(FVector::UpVector, SlopeNormal));
	if (SlopeAngle > MaxSlopeAngle)
	{
		float Lerp = (SlopeAngle - MaxSlopeAngle) / SlopeAngle;
		SlopeNormal = FMath::Lerp(SlopeNormal, FVector::UpVector, Lerp);
	}

	if(SlopeSmoothingRate < 0.0f)
	{
		CurrentSlopeNormal = SlopeNormal;
		CurrentSlopePoint = SlopePoint;
	}
	else
	{
		//Smooth the normal and slope point here
		const float MaxDelta = SlopeSmoothingRate * DeltaTime;

		//Smooth out the slope changes
		UStriderMath::MoveTowardVector(CurrentSlopeNormal, SlopeNormal, MaxDelta);
		UStriderMath::MoveTowardVector(CurrentSlopePoint, SlopePoint, MaxDelta * 20.0f);

		//Slope normal must be normalized otherwise the stride will be scaled
		if (CurrentSlopeNormal.SizeSquared() < 0.001f)
			CurrentSlopeNormal = FVector::UpVector;
		else
			CurrentSlopeNormal.Normalize();
	}
}

void FAnimNode_SlopeWarp::GatherDebugData(FNodeDebugData& DebugData)
{
	ComponentPose.GatherDebugData(DebugData);
}

bool FAnimNode_SlopeWarp::HasPreUpdate() const
{
	return SlopeDetectionMode == ESlopeDetectionMode::AutomaticSlope;
}

void FAnimNode_SlopeWarp::PreUpdate(const UAnimInstance* InAnimInstance)
{
	if (!InAnimInstance)
	{
		SlopeNormal = FVector::UpVector;
		SlopePoint = FVector::ZeroVector;

		return;
	}

	if (!CharMoveComponent || !Character)
	{
		Character = Cast<ACharacter>(InAnimInstance->GetOwningActor());

		if (Character)
			CharMoveComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());	
	}
	else if(CharMoveComponent->IsMovingOnGround())
	{
		const FTransform& CharTransform = Character->GetMesh()->GetComponentTransform();

		SlopeNormal = CharTransform.InverseTransformVector(
			CharMoveComponent->CurrentFloor.HitResult.Normal);

		SlopePoint = CharTransform.InverseTransformPosition(
			CharMoveComponent->CurrentFloor.HitResult.ImpactPoint);
	}
	else
	{
		//Reset the slope when not grounded
		SlopeNormal = FVector::UpVector;
		SlopePoint = FVector::ZeroVector;
	}
}

void FAnimNode_SlopeWarp::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	IkRoot.Initialize(RequiredBones);
	HipAdjustment.Initialize(RequiredBones);

	bool bLimbsValid = true;
	for (FLimbDefinition & LimbDef : Limbs)
	{
		LimbDef.Initialize(RequiredBones);

		if (!LimbDef.IsValid(RequiredBones))
		{
			bLimbsValid = false;
			break;
		}
	}

	for (FBoneReference& Bone : AdditionalBonesToAdjustWithHips)
	{
		Bone.Initialize(RequiredBones);

		if (!Bone.IsValidToEvaluate(RequiredBones))
		{
			UE_LOG(LogTemp, Warning, TEXT("Slope Warp Anim Node: Invalid 'additional bone' found in setup, this bone will not be affected,"))
		}
	}

	bValidCheckResult = HipAdjustment.IsValid(RequiredBones) 
		&& IkRoot.IsValidToEvaluate(RequiredBones)
		&& bLimbsValid;	

	if(bValidCheckResult)
	{
		const TArray<FTransform>& Transforms = AnimInstanceProxy->GetSkeleton()->GetRefLocalPoses();
		FCompactPoseBoneIndex ParentBoneCompactIndex = RequiredBones.GetParentBoneIndex(IkRoot.CachedCompactPoseIndex);

		const int32 IkRootBoneIndex = AnimInstanceProxy->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(IkRoot.BoneName);

		IKRootOffset = Transforms[IkRootBoneIndex].GetRotation();
		int32 BoneIndex = ParentBoneCompactIndex.GetInt();
		while (BoneIndex > -1)
		{
			IKRootOffset *= Transforms[BoneIndex].GetRotation();
			ParentBoneCompactIndex = RequiredBones.GetParentBoneIndex(ParentBoneCompactIndex);
			BoneIndex = ParentBoneCompactIndex.GetInt();
		}
	}
}