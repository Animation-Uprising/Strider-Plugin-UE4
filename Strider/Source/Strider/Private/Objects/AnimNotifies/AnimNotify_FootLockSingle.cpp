// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootLockSingle.h"

#include "FootLockManager.h"

#if ENGINE_MAJOR_VERSION >= 5
void UAnimNotify_FootLockSingle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
									   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void UAnimNotify_FootLockSingle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif

	if(UFootLockManager* FootLockManager = Cast<UFootLockManager>(MeshComp->GetOwner()->GetComponentByClass(UFootLockManager::StaticClass())))
	{
		if(bSetLocked)
		{
			FootLockManager->LockFoot(FootLockId, -1.0f);
		}
		else
		{
			FootLockManager->UnlockFoot(FootLockId); 
		}
	}
}
