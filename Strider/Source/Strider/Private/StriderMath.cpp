// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#include "..\Public\StriderMath.h"

float UStriderMath::MoveToward(const float Start, const float End, const float MaxDelta)
{
	if (FMath::Abs(End - Start) <= MaxDelta)
		return End;
	else
		return Start + FMath::Sign(End - Start) * MaxDelta;
}

float UStriderMath::MoveTowardAngle(float StartAngle, float EndAngle, const float MaxDelta)
{
	//First wrap the angles so that it is within -180 to 180
	StartAngle = WrapAngle(StartAngle);
	EndAngle = WrapAngle(EndAngle);

	//Then determine the direction and magnitude to move in 
	float AngleDelta = GetAngleDelta(StartAngle, EndAngle);

	float FinalAngle = EndAngle;
	if (FMath::Abs(AngleDelta) > MaxDelta)
	{
		FinalAngle = WrapAngle(StartAngle + FMath::Sign(AngleDelta) * MaxDelta);
	}

	return FinalAngle;
}

float UStriderMath::GetAngleDelta(const float StartAngle, const float EndAngle)
{
	float Delta = EndAngle - StartAngle;

	if (Delta > 180.0)
		Delta -= 360.0f;
	else if (Delta < -180.0f)
		Delta += 360.0f;

	return Delta;
}

void UStriderMath::MoveTowardVector(FVector& InStart, const FVector& End, const float MaxDelta)
{
	FVector Delta = End - InStart;
	float Dist = Delta.Size();

	if(Dist > 0.00001f)
		InStart += (Delta / Dist) * FMath::Min(MaxDelta, Dist);
}

void UStriderMath::MoveComponentsToward(FVector & InStart, const FVector & End, const float MaxDelta)
{
	FVector Diff = End - InStart;
	float DiffSum = Diff.X + Diff.Y + Diff.Z;
	Diff /= DiffSum;

	if (FMath::Abs(InStart.X - End.X) < (MaxDelta * Diff.X))
		InStart.X = End.X;
	else
		InStart.X += FMath::Sign(End.X - InStart.X) * (MaxDelta * Diff.X);
	
	if (FMath::Abs(InStart.Y - End.Y) < (MaxDelta * Diff.Y))
		InStart.Y = End.Y;
	else
		InStart.Y += FMath::Sign(End.Y - InStart.Y) * (MaxDelta * Diff.Y);

	if (FMath::Abs(InStart.Z - End.Z) < (MaxDelta * Diff.Z))
		InStart.Z = End.Z;
	else
		InStart.Z += FMath::Sign(End.Z - InStart.Z) * (MaxDelta * Diff.Z);
}

float UStriderMath::WrapAngle(float Angle)
{
	Angle = FMath::Fmod(Angle + 180.0f, 360.0f);
	if (Angle < 0.0f)
		Angle += 360.0f;

	Angle -= 180.0f;

	return Angle;
}

float UStriderMath::AngleBetween(const FVector& A, const FVector& B)
{
	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(A, B)));
}

float UStriderMath::CalculateCircleStrafeDirectionDelta(const float LastDirection, 
	const float Direction, const float DeltaTime)
{
	float DirectionDelta = GetAngleDelta(LastDirection, Direction);

	return FMath::Abs(DirectionDelta / DeltaTime) * FMath::Sign(LastDirection);
}

float UStriderMath::GetPointOnPlane(const FVector& Point, const FVector & SlopeNormal, const FVector & SlopeLocation)
{
	//z = (-a(X - X1) - b(y - y1)) / c) + z1
	return ((-(SlopeNormal.X * (Point.X - SlopeLocation.X))
			- (SlopeNormal.Y * (Point.Y - SlopeLocation.Y))) / SlopeNormal.Z)
			+ SlopeLocation.Z;
}

FVector UStriderMath::GetBoneWorldLocation(const FTransform& InBoneTransform_CS, FAnimInstanceProxy* InAnimInstanceProxy)
{
	return InAnimInstanceProxy->GetComponentTransform().TransformPosition(InBoneTransform_CS.GetLocation());
}

FVector UStriderMath::GetBoneWorldLocation(const FVector& InBoneLocation_CS, FAnimInstanceProxy* InAnimInstanceProxy)
{
	return InAnimInstanceProxy->GetComponentTransform().TransformPosition(InBoneLocation_CS);
}

float UStriderMath::CalculatePlayRate(const float TotalSpeedScale, const float PlaybackWeight,
	const float MinPlayRate, const float MaxPlayRate)
{
	return FMath::Clamp((TotalSpeedScale - 1.0f) * PlaybackWeight + 1.0f, MinPlayRate, MaxPlayRate);
}

float UStriderMath::CalculateStrideScale(const float TotalSpeedScale, const float PlayRate)
{
	return TotalSpeedScale / PlayRate;
}

float UStriderMath::GetRotationRelativeToVelocity(const AActor* Actor)
{
	if (!Actor)
		return 0.0f;

	FVector Velocity = Actor->GetVelocity();

	if (Velocity.SizeSquared() < 0.0001f)
		return 0.0f;

	float RotationZ = (Actor->GetActorRotation() - Velocity.ToOrientationRotator()).Yaw * -1.0f;

	if(RotationZ > 180.0f)
	{
		RotationZ -= 360.0f;
	}
	else if(RotationZ < -180.0f)
	{
		RotationZ += 360.0f;
	}

	return RotationZ;
}

int UStriderMath::GetNextCardinalDirection(const int CurrentCardinalDirection, const float RelativeDirection,
	const float StepDelta /*= 60.0f*/, const float SkipDelta /*= 135.0f*/)
{
	switch (CurrentCardinalDirection)
	{
	case 0: //North
	{
		if (RelativeDirection > StepDelta)
		{
			if (RelativeDirection > SkipDelta)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}
		else if (RelativeDirection < -StepDelta)
		{
			if (RelativeDirection < -SkipDelta)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}

	} break;

	case 1: //East
	{
		float OffsetDir = RelativeDirection - 90.0f;

		if (OffsetDir < -180.0f)
			OffsetDir += 360.0f;

		if (OffsetDir > StepDelta)
		{
			if (OffsetDir > SkipDelta)
			{
				return 3;
			}
			else
			{
				return 2;
			}
		}
		else if (OffsetDir < -StepDelta)
		{
			if (OffsetDir < -SkipDelta)
			{
				return 3;
			}
			else
			{
				return 0;
			}
		}


	} break;

	case 2: //South
	{
		float OffsetDir = RelativeDirection - 180.0f;

		if (OffsetDir < -180.0f)
		{
			OffsetDir += 360.0f;
		}
		else if (OffsetDir > 180.0f)
		{
			OffsetDir -= 360.0f;
		}

		if (OffsetDir > StepDelta)
		{
			if (OffsetDir > SkipDelta)
			{
				return 0;
			}
			else
			{
				return 3;
			}
		}
		else if (OffsetDir < -StepDelta)
		{
			if (OffsetDir < -SkipDelta)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}



	} break;

	case 3: //West
	{
		float OffsetDir = RelativeDirection + 90;

		if (OffsetDir > 180.0f)
			OffsetDir -= 360.0f;

		if (OffsetDir > StepDelta)
		{
			if (OffsetDir > SkipDelta)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else if (OffsetDir < -StepDelta)
		{
			if (OffsetDir < -SkipDelta)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}

	} break;
	}

	return CurrentCardinalDirection;
}