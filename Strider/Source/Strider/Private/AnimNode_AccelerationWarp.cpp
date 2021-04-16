// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "..\Public\AnimNode_AccelerationWarp.h"
#include "Animation/AnimInstanceProxy.h"
#include "StriderMath.h"

static TAutoConsoleVariable<int32> CVarAccelWarpEnabled(
	TEXT("a.AnimNode.Warp.Accel.Enable"),
	1,
	TEXT("Turns acceleration warp nodes on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: on \n"),
	ECVF_SetByConsole);

FAnimNode_AccelerationWarp::FAnimNode_AccelerationWarp()
	: Acceleration(0.0f),
	  Direction(0.0f),
	  Alpha(1.0f),
	  UpAxis(0.0f, 0.0f, 1.0f),
	  TorsoBendRatio(2.0f),
	  MaxTorsoBend(30.0f),
	  Smoothing(-1.0f),
	  CurrentAcceleration(0.0f),
	  bValidCheckResult(false)
{
}

void FAnimNode_AccelerationWarp::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	InputPose.Initialize(Context);
}

void FAnimNode_AccelerationWarp::Evaluate_AnyThread(FPoseContext & Output)
{
	InputPose.Evaluate(Output);

	if (!bValidCheckResult
		|| (Alpha < 0.0001f)
		|| !FMath::IsFinite(CurrentAcceleration)
		|| (FMath::Abs(CurrentAcceleration) < 0.0001f)
		|| (CVarAccelWarpEnabled.GetValueOnAnyThread() == 0)
		|| !IsLODEnabled(Output.AnimInstanceProxy))
	{
		return;
	}

	FQuat AnchorRotation_CS = Output.Pose[FCompactPoseBoneIndex(0)].GetRotation();

	float BendAngle = FMath::DegreesToRadians(FMath::Clamp(TorsoBendRatio * CurrentAcceleration,
		-MaxTorsoBend, MaxTorsoBend) * Alpha);

	//FVector BendAxis = FQuat(FVector::UpVector, FMath::DegreesToRadians(Direction)) * FVector::BackwardVector;

	FVector BendAxis = FQuat(UpAxis, FMath::DegreesToRadians(Direction)) * FVector::BackwardVector;

	SpineChain.CalculateAnchorRotation(AnchorRotation_CS, Output.Pose);
	SpineChain.ApplyComponentSpaceRotation(Output.Pose, AnchorRotation_CS, 
		BendAxis, BendAngle, Alpha);
}

void FAnimNode_AccelerationWarp::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	InputPose.CacheBones(Context);

	const FBoneContainer& BoneContainer = Context.AnimInstanceProxy->GetRequiredBones();
	SpineChain.Initialize(BoneContainer);
	ValidateData(BoneContainer);
}

void FAnimNode_AccelerationWarp::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	GetEvaluateGraphExposedInputs().Execute(Context);
	InputPose.Update(Context);

	if(Smoothing > 0.0f)
	{
		CurrentAcceleration = UStriderMath::MoveToward(CurrentAcceleration,
			Acceleration, Smoothing * Context.GetDeltaTime());
	}
	else
	{
		CurrentAcceleration = Acceleration;
	}
}

bool FAnimNode_AccelerationWarp::ValidateData(const FBoneContainer& BoneContainer)
{
	bValidCheckResult = SpineChain.ValidateData(BoneContainer);

	return bValidCheckResult;
}

void FAnimNode_AccelerationWarp::GatherDebugData(FNodeDebugData& DebugData)
{
	DebugData.AddDebugItem(DebugData.GetNodeName(this));
	InputPose.GatherDebugData(DebugData);
}
