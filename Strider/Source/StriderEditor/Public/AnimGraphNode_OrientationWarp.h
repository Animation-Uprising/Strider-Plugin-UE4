// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AnimNode_OrientationWarp.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_OrientationWarp.generated.h"


/** An editor animation node for orientation warping on characters */
UCLASS()
class STRIDEREDITOR_API UAnimGraphNode_OrientationWarp : public UAnimGraphNode_Base
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Runtime")
	FAnimNode_OrientationWarp OrientationWarpNode;

public:
	UAnimGraphNode_OrientationWarp();

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
	//~ End UAnimGraphNode_Base Interface
};