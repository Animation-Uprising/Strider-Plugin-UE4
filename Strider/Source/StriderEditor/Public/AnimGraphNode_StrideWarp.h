// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AnimNode_StrideWarp.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_StrideWarp.generated.h"

/** An editor animation node for warping/scaling the stride of characters */
UCLASS()
class STRIDEREDITOR_API UAnimGraphNode_StrideWarp : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Runtime")
	FAnimNode_StrideWarp StrideWarpNode;

public:
	UAnimGraphNode_StrideWarp();

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const;
	// End of UAnimGraphNode_SkeletalControlBase interface
};