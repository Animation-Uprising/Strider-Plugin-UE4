// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AnimNode_FootLocker.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_FootLocker.generated.h"

/** An editor animation node for warping/scaling the stride of characters */
UCLASS()
class STRIDEREDITOR_API UAnimGraphNode_FootLocker : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Runtime")
	FAnimNode_FootLocker FootLockerNode;

public:
	UAnimGraphNode_FootLocker();

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