// Fill out your copyright notice in the Description page of Project Settings.


#include "FootLockManager.h"


FFootLockData::FFootLockData()
{
}

FFootLockData::FFootLockData(const EFootLockState InLockState, const float InRemainingLockTime)
	: LockState(InLockState), RemainingLockTime(InRemainingLockTime)
{
	
}

// Sets default values for this component's properties
UFootLockManager::UFootLockManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FootLockMap.Empty(static_cast<int32>(EFootLockId::Foot8) + 1);
}

// Called every frame
void UFootLockManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Update the lock timers on on valid feet and unlock them if necessary
	for(TPair<EFootLockId, FFootLockData>& FootLockPair : FootLockMap)
	{
		FFootLockData& FootLockData = FootLockPair.Value;

		if(FootLockData.LockState == EFootLockState::TimeLocked)
		{
			FootLockData.RemainingLockTime -= DeltaTime;

			if(FootLockData.RemainingLockTime < 0.0f)
			{
				FootLockData.LockState = EFootLockState::Unlocked;
				FootLockData.RemainingLockTime = 0.0f;
			}
		}
	}
}

void UFootLockManager::LockFoot(const EFootLockId FootId, const float Duration)
{
	FFootLockData& FootLockData = FootLockMap.FindOrAdd(FootId);

	if(Duration > 0.0f)
	{
		FootLockData.LockState = EFootLockState::TimeLocked;
	}
	else
	{
		FootLockData.LockState = EFootLockState::Locked;
	}

	FootLockData.RemainingLockTime = Duration;
}

void UFootLockManager::UnlockFoot(const EFootLockId FootId)
{
	if(FFootLockData* FootLockData = FootLockMap.Find(FootId))
	{
		FootLockData->LockState = EFootLockState::Unlocked;
		FootLockData->RemainingLockTime = 0.0f;
	}
}

bool UFootLockManager::IsFootLocked(const EFootLockId FootId) const
{
	if(const FFootLockData* FootLockData = FootLockMap.Find(FootId))
	{
		return FootLockData->LockState > EFootLockState::Unlocked;
	}

	return false;
}

void UFootLockManager::ResetLockingState()
{
	for(TPair<EFootLockId, FFootLockData>& FootLockPair : FootLockMap)
	{
		FFootLockData& FootLockData = FootLockPair.Value;

		FootLockData.LockState = EFootLockState::Unlocked;
		FootLockData.RemainingLockTime = 0.0f;
	}
}

