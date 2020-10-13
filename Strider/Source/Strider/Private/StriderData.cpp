// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "StriderData.h"
#include "StriderMath.h"

FStridePivot::FStridePivot()
	: bProjectToGround(false),
	  Offset(0.0f),
	  StrideVectorMethod(EStrideVectorMethod::ManualVelocity),
	  Smoothing(-1.0f),
	  bChooseNearestAxis(true),
	  CurrentDirection(0.0f),
	  Transform(FTransform::Identity)
{ 
}

void FStridePivot::Initialize(const FBoneContainer& Bones)
{
	Root.Initialize(Bones);
}

bool FStridePivot::IsValid(const FBoneContainer & Bones)
{
	return Root.IsValidToEvaluate(Bones);
}

FQuat FStridePivot::GetDirectionRotation(float Direction, const float DeltaTime)
{
	Direction = UStriderMath::WrapAngle(Direction);

	//Choose nearest axis (i.e. the direction isn't so important it's more the axis of the direction)
	if (bChooseNearestAxis)
	{
		float AltDirection = Direction - FMath::Sign(Direction) * 180.0f;

		if (FMath::Abs(AltDirection - CurrentDirection)
			< FMath::Abs(Direction - CurrentDirection))
		{
			Direction = AltDirection;
		}
	}

	//Smooth the direction change so the IK doesn't pop
	if(Smoothing > 0.0f)
		CurrentDirection = UStriderMath::MoveToward(CurrentDirection, Direction, Smoothing * DeltaTime);
	else
		CurrentDirection = Direction;


	return FQuat(FVector::UpVector, FMath::DegreesToRadians(CurrentDirection));
}

FHipAdjustment::FHipAdjustment()
	: AdjustmentRatio(1.0f),
	 MaxRecoveryRate(-1.0f)
{
}

void FHipAdjustment::Initialize(const FBoneContainer& Bones)
{
	Hips.Initialize(Bones);
}

bool FHipAdjustment::IsValid(const FBoneContainer& Bones)
{
	return Hips.IsValidToEvaluate(Bones);
}

FLimbDefinition::FLimbDefinition()
	: BoneCount(2),
	  Length(-1.0f),
	  HeightDelta(0.0f),
	  TipLocation_CS(FVector::ZeroVector),
	  BaseBoneTransform_CS(FTransform::Identity)
{
}

void FLimbDefinition::Initialize(const FBoneContainer& PoseBones)
{
	Length = -1.0f;

	//Initialize set bones
	Tip.Initialize(PoseBones);
	IkTarget.Initialize(PoseBones);

	Bones.Empty(BoneCount);

	if (!Tip.IsValidToEvaluate(PoseBones))
		return;

	const FReferenceSkeleton& RefSkeleton = PoseBones.GetReferenceSkeleton();

	int32 ParentBoneIndex = PoseBones.GetParentBoneIndex(Tip.BoneIndex);
	for (int32 i = 0; i < BoneCount; ++i)
	{
		if(ParentBoneIndex == 0)
		{
			break;
		}

		FBoneReference Bone = FBoneReference(RefSkeleton.GetBoneName(ParentBoneIndex));
		Bone.Initialize(PoseBones);
		Bones.Add(Bone);

		ParentBoneIndex = PoseBones.GetParentBoneIndex(ParentBoneIndex);
	}
}

bool FLimbDefinition::IsValid(const FBoneContainer& PoseBones)
{
	bool bIsValid = (IkTarget.IsValidToEvaluate(PoseBones)
		&& Tip.IsValidToEvaluate(PoseBones))
		&& (Bones.Num() == BoneCount)
		&& (BoneCount > 1);

	for(FBoneReference& Bone : Bones)
	{
		if (!Bone.IsValidToEvaluate(PoseBones))
		{
			bIsValid = false;
			break;
		}
	}

	return bIsValid;
}

void FLimbDefinition::CalculateLength(FCSPose<FCompactPose>& Pose)
{
	//Length from foot to previous bone hinge
	Length = Pose.GetLocalSpaceTransform(Tip.CachedCompactPoseIndex).GetLocation().Size();

	//Length from subsequent bones to their previous bone up the chain (Base bone excluded)
	for (int32 i = 0; i < BoneCount -1; ++i)
	{
		Length += Pose.GetLocalSpaceTransform(Bones[i].CachedCompactPoseIndex).GetLocation().Size();
	}
}
