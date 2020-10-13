// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "AnimGraphNode_SlopeWarp.h"
//#include "UnrealWidget.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_SlopeWarp::UAnimGraphNode_SlopeWarp()
{
}

FText UAnimGraphNode_SlopeWarp::GetTooltipText() const
{
	return LOCTEXT("SlopeWarp", "Slope Warp");
}

FLinearColor UAnimGraphNode_SlopeWarp::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_SlopeWarp::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("SlopeWarp", "Slope Warp");
}

FString UAnimGraphNode_SlopeWarp::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

const FAnimNode_SkeletalControlBase* UAnimGraphNode_SlopeWarp::GetNode() const
{
	return &SlopeWarpNode;
}

#undef LOCTEXT_NAMESPACE