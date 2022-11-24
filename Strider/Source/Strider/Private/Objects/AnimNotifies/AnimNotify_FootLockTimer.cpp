// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_FootLockTimer.h"
#include "FootLockManager.h"


void UAnimNotify_FootLockTimer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if(UFootLockManager* FootLockManager = Cast<UFootLockManager>(MeshComp->GetOwner()->GetComponentByClass(UFootLockManager::StaticClass())))
	{
		FootLockManager->LockFoot(FootLockId, GroundingTime);
	}
}