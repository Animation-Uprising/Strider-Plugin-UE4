// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "AnimGraphNode_AccelerationWarp.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_AccelerationWarp::UAnimGraphNode_AccelerationWarp()
{

}

FLinearColor UAnimGraphNode_AccelerationWarp::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_AccelerationWarp::GetTooltipText() const
{
	return LOCTEXT("AccelerationWarpNode", "Acceleration Warp Node");
}

FText UAnimGraphNode_AccelerationWarp::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AccelerationWarp", "Acceleration Warp");
}

FString UAnimGraphNode_AccelerationWarp::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

#undef LOCTEXT_NAMESPACE