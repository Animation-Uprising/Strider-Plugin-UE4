// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EStriderEnumerations.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootLockSingle.generated.h"

/**
 * 
 */
UCLASS()
class STRIDER_API UAnimNotify_FootLockSingle : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	/** Which foot should be locked by this notify*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot Locker")
	EFootLockId FootLockId;

	/** How long should the foot be locked for in seconds*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foot Locker", meta = (ClampMin = 0.0f))
	bool bSetLocked;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};