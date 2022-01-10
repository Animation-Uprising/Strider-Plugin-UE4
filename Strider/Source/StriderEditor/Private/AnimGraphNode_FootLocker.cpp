// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "AnimGraphNode_FootLocker.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_FootLocker::UAnimGraphNode_FootLocker()
{
}

FText UAnimGraphNode_FootLocker::GetTooltipText() const
{
	return LOCTEXT("FootLocker", "Foot Locker");
}

FLinearColor UAnimGraphNode_FootLocker::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_FootLocker::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FootLocker", "Foot Locker");
}

FString UAnimGraphNode_FootLocker::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

const FAnimNode_SkeletalControlBase* UAnimGraphNode_FootLocker::GetNode() const
{
	return &FootLockerNode;
}

#undef LOCTEXT_NAMESPACE