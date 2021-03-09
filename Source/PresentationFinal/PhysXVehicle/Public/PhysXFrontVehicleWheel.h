// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "PhysXFrontVehicleWheel.generated.h"

/**
 * 
 */


PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS()
class PRESENTATIONFINAL_API UPhysXFrontVehicleWheel : public UVehicleWheel
{
	GENERATED_BODY()

public:
	UPhysXFrontVehicleWheel();

	//void SetSuspensionDampingRatio(float inRatio);
	
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS