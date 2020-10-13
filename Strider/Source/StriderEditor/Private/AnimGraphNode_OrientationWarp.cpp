// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "AnimGraphNode_OrientationWarp.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_OrientationWarp::UAnimGraphNode_OrientationWarp()
{
}

FLinearColor UAnimGraphNode_OrientationWarp::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_OrientationWarp::GetTooltipText() const
{
	return LOCTEXT("OrientationWarpNode", "Orientation Warp Node");
}

FText UAnimGraphNode_OrientationWarp::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("OrientationWarp", "Orientation Warp");
}

FString UAnimGraphNode_OrientationWarp::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

#undef LOCTEXT_NAMESPACE