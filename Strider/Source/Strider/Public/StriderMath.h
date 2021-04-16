// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StriderMath.generated.h"

UCLASS()
class STRIDER_API UStriderMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Moves a float value towards another float value by a max delta without overshooting */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static float MoveToward(const float Start, const float End, const float MaxDelta);

	/** Moves a vector toward another vector by a max delta without overshooting. */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static void MoveTowardVector(FVector& InStart, const FVector& End, const float MaxDelta);

	/** Moves a vector toward another vector by a max delta without overshooting. This is an approximate
	method which moves the vector on a 'pre component (xyz)' basis and should not be relied upon for 
	accuracy. */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static void MoveComponentsToward(FVector& InStart, const FVector& End, const float MaxDelta);

	/** Moves an angle value (in degrees) towards another angle value in the shortest direction, by a max
	delta without overshooting. */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static float MoveTowardAngle(float StartAngle, float EndAngle, const float MaxDelta);

	/** Wraps an angle (degrees) so that it is within -180 to 180 */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static float WrapAngle(float Angle);

	/** Returns the difference between two angles (degrees) in the shortest direction. 
	Assumes the passed angles are within the -180 to 180 degree range*/
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static float GetAngleDelta(const float StartAngle, const float EndAngle);

	/** Returns the shortest angle (degrees) beteen two vectors */
	UFUNCTION(BlueprintCallable, Category = "Strider Math")
	static float AngleBetween(const FVector& A, const FVector& B);

	/** Calculates a direction delta (in degrees) to inform lean warping when circle strafing */
	UFUNCTION(BlueprintCallable, Category = "Animation Warping Utility")
	static float CalculateCircleStrafeDirectionDelta(const float LastDirection, const float Direction, const float DeltaTime);
	
	/** Calculates the play rate on total speed scale and playback weight*/
	UFUNCTION(BlueprintCallable, Category = "Animation Warping Utility")
	static float CalculatePlayRate(const float TotalSpeedScale, const float PlaybackWeight,
			const float MinPlayRate, const float MaxPlayRate);

	/** Given the total speed scale and the calculated playrate this function returns the stride scale. */
	UFUNCTION(BlueprintCallable, Category = "Animation Warping Utility")
	static float CalculateStrideScale(const float TotalSpeedScale, const float PlayRate);

	/** Gets the rotation of an actor relative to it's velocity.*/
	UFUNCTION(BlueprintCallable, Category = "Animation Warping Utility")
	static float GetRotationRelativeToVelocity(const AActor* Actor);

	/** Given an input Cardinal Direction, 0-N, 1-E, 2-S, 3-W, and the relative direction, this function
	determines which cardinal direction to switch to. Note: This function expectes the RelativeDirection to be within the range -180 to 180 degrees*/
	UFUNCTION(BlueprintCallable, Category = "Animation Warping Utility")
	static int GetNextCardinalDirection(const int CurrentCardinalDirection, const float RelativeDirection,
			const float StepDelta = 60.0f, const float SkipDelta = 135.0f);

	static float GetPointOnPlane(const FVector& InPoint, const FVector& SlopeNormal, const FVector& SlopeLocation);
	static FVector GetBoneWorldLocation(const FTransform& InBoneTransform_CS, FAnimInstanceProxy* InAnimInstanceProxy);
	static FVector GetBoneWorldLocation(const FVector& InBoneLocation_CS, FAnimInstanceProxy* InAnimInstanceProxy);

};