// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EStriderEnumerations.h"
#include "Components/ActorComponent.h"
#include "FootLockManager.generated.h"

UENUM(BlueprintType)
enum class EFootLockState : uint8
{
	Unlocked,
	Locked,
	TimeLocked
};


USTRUCT()
struct STRIDER_API FFootLockData
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	EFootLockState LockState = EFootLockState::Unlocked;

	UPROPERTY(Transient)
	float RemainingLockTime = 0.0f;

public:
	FFootLockData();
	FFootLockData(const EFootLockState InLockState, const float InRemainingLockTime);
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRIDER_API UFootLockManager : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient)
	TMap<EFootLockId, FFootLockData> FootLockMap;

public:	
	UFootLockManager();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Strider|FootLocker")
	void LockFoot(const EFootLockId FootId, const float Duration);

	UFUNCTION(BlueprintCallable, Category = "Strider|FootLocker")
	void UnlockFoot(const EFootLockId FootId);

	UFUNCTION(BlueprintCallable, Category = "Strider|FootLocker")
	bool IsFootLocked(const EFootLockId FootId) const;

	UFUNCTION(BlueprintCallable, Category = "Strider|FootLocker")
	void ResetLockingState();
};
