// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysXRearVehicleWheel.h"
#include "UObject/ConstructorHelpers.h"
#include "TireConfig.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS


UPhysXRearVehicleWheel::UPhysXRearVehicleWheel()
{
	//Class for the rear wheels of a 
	//It is affected by the handbrake


	ShapeRadius = 50.f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = true;
	

	//Suspension force applied vertically
	SuspensionForceOffset = 0.f;
	SuspensionMaxRaise = 4.0f; //compression
	SuspensionMaxDrop = 15.0f; //extension
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;

	//GetSuspensionOffset() returns compression as +ve and extension as -ve
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS
