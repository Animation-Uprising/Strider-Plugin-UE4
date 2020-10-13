// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "AnimGraphNode_BankWarp.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_BankWarp::UAnimGraphNode_BankWarp()
{

}

FLinearColor UAnimGraphNode_BankWarp::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_BankWarp::GetTooltipText() const
{
	return LOCTEXT("BankWarpNode", "Bank Warp Node");
}

FText UAnimGraphNode_BankWarp::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("BankWarp", "Bank Warp");
}

FString UAnimGraphNode_BankWarp::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

#undef LOCTEXT_NAMESPACE