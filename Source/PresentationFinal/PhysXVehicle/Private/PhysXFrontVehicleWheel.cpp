// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysXFrontVehicleWheel.h"
#include "TireConfig.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS

UPhysXFrontVehicleWheel::UPhysXFrontVehicleWheel()
{
	//Class for the front wheels of a 
	//It isn't affected by the handbrake

	ShapeRadius = 50.f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = false;
	SteerAngle = 40.f;


	SuspensionForceOffset = 0.f;
	SuspensionMaxRaise = 4.0f;
	SuspensionMaxDrop = 15.0f;
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;
}


PRAGMA_ENABLE_DEPRECATION_WARNINGS