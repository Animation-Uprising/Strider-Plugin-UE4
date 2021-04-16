// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "..\Public\AnimNode_OrientationWarp.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "BonePose.h"
#include "StriderMath.h"

static TAutoConsoleVariable<int32> CVarOrientWarpEnabled(
	TEXT("a.AnimNode.Warp.Orient.Enable"),
	1,
	TEXT("Turns orientation warp nodes on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: on \n"),
	ECVF_SetByConsole);

FAnimNode_OrientationWarp::FAnimNode_OrientationWarp()
	: Direction(0.0f),
	  Offset(0.0f),
	  UpperBodyAlpha(1.0f),
	  UpAxis(FVector(0.0f, 0.0f, 1.0f)),
	  Alpha(1.0f),
	  MaxWarpDelta(60.0f),
	  Smoothing(-1.0f),
	  CurrentDirection(0.0f),
	  bValidCheckResult(false)
{
}

void FAnimNode_OrientationWarp::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	InputPose.Initialize(Context);
}

void FAnimNode_OrientationWarp::Evaluate_AnyThread(FPoseContext & Output)
{
	InputPose.Evaluate(Output);

	if (!bValidCheckResult
		|| (Alpha < 0.0001f)
		|| !FMath::IsFinite(CurrentDirection)
		|| (FMath::Abs(CurrentDirection) < 0.0001f)
		|| (CVarOrientWarpEnabled.GetValueOnAnyThread() == 0)
		|| !IsLODEnabled(Output.AnimInstanceProxy))
	{ 
		return;
	}

	float CurDirectionRad = FMath::DegreesToRadians(CurrentDirection) * Alpha;

	//Rotate Root Bone around the Z axis by the current direction. (This will rotate the entire model)
	FTransform& RootTransform = Output.Pose[RootBone.CachedCompactPoseIndex];
	FQuat AnchorRotation_CS = RootTransform.GetRotation();
	RootTransform.SetRotation(AnchorRotation_CS * FQuat(UpAxis, CurDirectionRad));
	RootTransform.NormalizeRotation();

	if ((UpperBodyAlpha < 0.0001f) 
		|| (SpineChain.BoneChain.Num() == 0))
	{
		return;
	}

	SpineChain.CalculateAnchorRotation(AnchorRotation_CS, Output.Pose);
	SpineChain.ApplyComponentSpaceRotation(Output.Pose, AnchorRotation_CS, UpAxis, 
		-CurDirectionRad, UpperBodyAlpha);

	//TODO: This rotation is being done in local space. It may not adequately accommodate bones other than the IK root
	for (FBoneReference& Bone : RootBonesToCounterAdjust)
	{
		FTransform& BoneTransform = Output.Pose[Bone.CachedCompactPoseIndex];
		BoneTransform.SetRotation(BoneTransform.GetRotation() * FQuat(UpAxis, -CurDirectionRad));
		RootTransform.NormalizeRotation();
	}
}

void FAnimNode_OrientationWarp::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	InputPose.CacheBones(Context);

	const FBoneContainer& BoneContainer = Context.AnimInstanceProxy->GetRequiredBones();
	RootBone.Initialize(BoneContainer);
	SpineChain.Initialize(BoneContainer);

	for (FBoneReference& Bone : RootBonesToCounterAdjust)
	{
		Bone.Initialize(BoneContainer);
	}


	ValidateData(BoneContainer);
}

void FAnimNode_OrientationWarp::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	GetEvaluateGraphExposedInputs().Execute(Context);
	InputPose.Update(Context);

	UpAxis.Normalize();

	if (FMath::Abs(UStriderMath::GetAngleDelta(-Offset, UStriderMath::WrapAngle(Direction))) < MaxWarpDelta)
	{
		//Wrap and clamp the direction then smooth it.
		Direction = UStriderMath::WrapAngle(Direction + Offset);

		if (Smoothing > 0.0f)
		{
			CurrentDirection = UStriderMath::MoveToward(CurrentDirection,
				Direction, Smoothing * Context.GetDeltaTime());
		}
		else
		{
			CurrentDirection = Direction;
		}
	}
}

bool FAnimNode_OrientationWarp::ValidateData(const FBoneContainer & BoneContainer)
{
	bValidCheckResult = RootBone.IsValidToEvaluate(BoneContainer)
		&& SpineChain.ValidateData(BoneContainer);

	for (FBoneReference& Bone : RootBonesToCounterAdjust)
	{
		if (!Bone.IsValidToEvaluate(BoneContainer))
		{
			bValidCheckResult = false;
			UE_LOG(LogTemp, Warning, TEXT("Orientation Warp Anim Node: Invalid 'additional bone' found in setup, orientation warping disabled."))
		}
	}

	return bValidCheckResult;
}

void FAnimNode_OrientationWarp::GatherDebugData(FNodeDebugData& DebugData)
{
	DebugData.AddDebugItem(DebugData.GetNodeName(this));
	InputPose.GatherDebugData(DebugData);
}