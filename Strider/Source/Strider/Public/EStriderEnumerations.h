// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "EStriderEnumerations.generated.h"

/** An enumeration defining the various options for getting the stride warp vector */
UENUM()
enum EStrideVectorMethod
{
	ManualVelocity UMETA(DisplayName = "Manual Input", ToolTip = "Stride direction must be set manually in the animation graph, either by a pin or fixed settigns."),
	ActorVelocity UMETA(DisplayName = "Actor Velocity", ToolTip = "Stride direction will be automatically calculated from the actor's current velocity.")
};

/** An enumeration defining the options for slope detection on the slope warp node */
UENUM()
enum ESlopeDetectionMode
{
	ManualSlope UMETA(DisplayName = "Manual", Tooltip = "The user must manually input the slope normal and contact point."),
	AutomaticSlope UMETA(DisplayName = "Automatic", Tooltip = "The slope normal and contact point will be automatically detected via the character controller capsule.")
};

/** An enumeration defining the different methods for compensating for slope roll on the slope warp node*/
UENUM()
enum ESlopeRollCompensation
{
	None,
	AdjustHips,
	AdjustFeet
};
