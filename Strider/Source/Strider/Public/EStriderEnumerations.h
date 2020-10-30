// Copyright 2020 Kenneth Claassen, All Rights Reserved.
#pragma once

#include "EStriderEnumerations.generated.h"

/** An enumeration defining the various options for getting the stride warp vector */
UENUM()
enum class EStrideVectorMethod : uint8
{
	ManualVelocity UMETA(DisplayName = "Manual Input", ToolTip = "Stride direction must be set manually in the animation graph, either by a pin or fixed settigns."),
	ActorVelocity UMETA(DisplayName = "Actor Velocity", ToolTip = "Stride direction will be automatically calculated from the actor's current velocity.")
};

/** An enumeration defining the options for slope detection on the slope warp node */
UENUM()
enum class ESlopeDetectionMode : uint8
{
	ManualSlope UMETA(DisplayName = "Manual", Tooltip = "The user must manually input the slope normal and contact point."),
	AutomaticSlope UMETA(DisplayName = "Automatic", Tooltip = "The slope normal and contact point will be automatically detected via the character controller capsule.")
};

/** An enumeration defining the different methods for compensating for slope roll on the slope warp node*/
UENUM()
enum class ESlopeRollCompensation : uint8
{
	None UMETA(DisplayName = "None", Tooltip = "No compensation for slope roll will take be made."),
	AdjustHips UMETA(DisplayName = "Adjust Hips", Tooltip = "Hips will be shifted over the feet to compensate for slope roll"),
	AdjustFeet UMETA(DisplayName = "Adjust Feet", Tooltip = "Feet will be shifted to compensate for slope roll")
};

/** An enumeration used for slope warp quality levels*/
UENUM()
enum class ESlopeWarpQuality : uint8
{
	Capsule UMETA(DisplayName = "Capsule Based", Tooltip = "Gate will be adjusted based on an average slope normal detected by the character capsule."),
	PerFootRay UMETA(DisplayName = "Per Foot Ray Trace", Tooltip = "Ray traces will be case from each foot to determine the slope position and normal per foot and adjust accordingly."),
	PerFootShape UMETA(DisplayName = "Per Foot Shape Trace", Tooltip = "shape traces will be cast from each foot to determine the slope position and normal per foot and adjust accordingly."),
	LODBased UMETA(DisplayName = "Dynamic - LOD Based", Tooltip = "Different quality levels will be used for different LOD's of the character. See LOD section for configuration.")
};