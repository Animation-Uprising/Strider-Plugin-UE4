// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "..\Public\AnimNode_BankWarp.h"
#include "Animation/AnimInstanceProxy.h"
#include "StriderMath.h"

static TAutoConsoleVariable<int32> CVarBankWarpEnabled(
	TEXT("a.AnimNode.Warp.Bank.Enable"),
	1,
	TEXT("Turns bank warp nodes on or off. \n")
	TEXT("<=0: off \n")
	TEXT("  1: on \n"),
	ECVF_SetByConsole);

FAnimNode_BankWarp::FAnimNode_BankWarp()
	: BankValue(0.0f),
	  Alpha(1.0f),
	  UpAxis(FVector(0.0f, 0.0f, 1.0f)),
	  ForwardAxis(FVector(0.0f, -1.0f, 0.0f)),
	  TwistRate(2.0f),
	  MaxTwist(30.0f),
	  LeanRate(2.0f),
	  MaxLean(15.0f),
	  Smoothing(-1.0f),
      bValidCheckResult(false)
{
}

void FAnimNode_BankWarp::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	InputPose.Initialize(Context);
}

void FAnimNode_BankWarp::Evaluate_AnyThread(FPoseContext & Output)
{
	InputPose.Evaluate(Output);

	if (!bValidCheckResult
		|| (Alpha < 0.0001f)
		|| !FMath::IsFinite(CurrentBankValue)
		|| (FMath::Abs(CurrentBankValue) < 0.0001f)
		|| (CVarBankWarpEnabled.GetValueOnAnyThread() == 0)
		|| !IsLODEnabled(Output.AnimInstanceProxy))
	{
		return;
	}
		
	FTransform& RootTransform = Output.Pose[RootBone.CachedCompactPoseIndex];
	FQuat AnchorRotation_CS = RootTransform.GetRotation();

	//Twist First
	if ((MaxTwist > 0.0001f)
		&& SpineChain.BoneChain.Num() > 0)
	{
		float TwistRad = FMath::DegreesToRadians(FMath::Clamp(TwistRate * CurrentBankValue,
			-MaxTwist, MaxTwist) * Alpha);

		SpineChain.CalculateAnchorRotation(AnchorRotation_CS, Output.Pose);
		SpineChain.ApplyComponentSpaceRotation(Output.Pose, AnchorRotation_CS,
			UpAxis, TwistRad, 1.0f);

		//TODO: This rotation is being done in local space. It may not adequately accommodate bones other than the IK root		
		for (FBoneReference& Bone : RootBonesToAdjust)
		{
			FTransform& BoneTransform = Output.Pose[Bone.CachedCompactPoseIndex];
			BoneTransform.SetRotation(BoneTransform.GetRotation() * FQuat(UpAxis, TwistRad));
			BoneTransform.NormalizeRotation();
		}
	}
	
	if (MaxLean > 0.0001f)
	{
		//Lean Second
		float LeanRad = FMath::DegreesToRadians(FMath::Clamp(LeanRate * CurrentBankValue,
			-MaxLean, MaxLean) * Alpha);

		RootTransform.ConcatenateRotation(FQuat(ForwardAxis, LeanRad));
		RootTransform.NormalizeRotation();
	}
}	

void FAnimNode_BankWarp::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	InputPose.CacheBones(Context);

	const FBoneContainer& BoneContainer = Context.AnimInstanceProxy->GetRequiredBones();
	RootBone.Initialize(BoneContainer);
	SpineChain.Initialize(BoneContainer);

	for (FBoneReference& Bone : RootBonesToAdjust)
	{
		Bone.Initialize(BoneContainer);
	}


	ValidateData(BoneContainer);
}

void FAnimNode_BankWarp::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	GetEvaluateGraphExposedInputs().Execute(Context);
	InputPose.Update(Context);

	if(Smoothing > 0.0f)
	{
		CurrentBankValue = UStriderMath::MoveToward(CurrentBankValue,
			BankValue, Smoothing * Context.GetDeltaTime());
	}
	else
	{
		CurrentBankValue = BankValue;
	}
}

void FAnimNode_BankWarp::GatherDebugData(FNodeDebugData& DebugData)
{
	DebugData.AddDebugItem(DebugData.GetNodeName(this));
	InputPose.GatherDebugData(DebugData);

}

bool FAnimNode_BankWarp::ValidateData(const FBoneContainer& BoneContainer)
{
	bValidCheckResult = SpineChain.ValidateData(BoneContainer)
		&& RootBone.IsValidToEvaluate(BoneContainer);

	for (FBoneReference& Bone : RootBonesToAdjust)
	{
		if (!Bone.IsValidToEvaluate(BoneContainer))
		{
			bValidCheckResult = false;
			UE_LOG(LogTemp, Warning, TEXT("Bank Warp Anim Node: Invalid 'additional bone' found in setup, Bank warping disabled."));
		}
	}

	return bValidCheckResult;
}