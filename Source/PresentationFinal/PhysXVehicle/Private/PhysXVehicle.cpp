// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysXVehicle.h"
#include "UObject/ConstructorHelpers.h"
#include "WheeledVehicleMovementComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Containers/Map.h"
#include "TimerManager.h"
#include "TireConfig.h"
#include "SkeletalRenderPublic.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Engine/SkeletalMesh.h"
#include "StaticMeshResources.h"
#include "PhysXRearVehicleWheel.h"
#include "PhysXFrontVehicleWheel.h"
#include "Components/SceneComponent.h"
#include "PhysXVehicleManager.h"
#include "PresentationFinalCharacter.h"

//
//FSuspensionOffsets SuspensionOffsets;

//FWheelSetup FrontLeftWheel;
//FWheelSetup FrontRightWheel;
//FWheelSetup RearLeftWheel;
//FWheelSetup RearRightWheel;


PRAGMA_DISABLE_DEPRECATION_WARNINGS


APhysXVehicle::APhysXVehicle()
{

	//AddToRoot();

	SportsCarSKMesh = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Game/VehicleVarietyPack/Skeletons/SK_SportsCar.SK_SportsCar")).Object;
	if (SportsCarSKMesh)
	{
		GetMesh()->SetSkeletalMesh(SportsCarSKMesh);
	}

	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = true;
	bReplicates = true;

	Vehicle = Cast<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());


	//Default setup taken from the engine
	
	Vehicle->MaxEngineRPM = 5700.0f;
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	//default steering
	Vehicle->SteeringCurve.GetRichCurve()->Reset();
	Vehicle->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Vehicle->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);


	
	Vehicle->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	Vehicle->Mass = 1500.0f;

	/*Wheel and suspension set up values taken from static class constructor
	therefore cannot be changed at run time without engine modifications
	*/
	
	FWheelSetup FrontLeftWheel = Vehicle->WheelSetups[0];
	FrontLeftWheel.WheelClass = UPhysXFrontVehicleWheel::StaticClass();
	FrontLeftWheel.BoneName = "Front_Left_Wheel";
	
	FWheelSetup FrontRightWheel = Vehicle->WheelSetups[1];
	FrontRightWheel.WheelClass = UPhysXFrontVehicleWheel::StaticClass();
	FrontRightWheel.BoneName = "Front_Right_Wheel";

	FWheelSetup RearLeftWheel = Vehicle->WheelSetups[2];
	RearLeftWheel.WheelClass = UPhysXRearVehicleWheel::StaticClass();
	RearLeftWheel.BoneName = "Rear_Left_Wheel";
	RearLeftWheel.bDisableSteering = true;

	FWheelSetup RearRightWheel = Vehicle->WheelSetups[3];
	RearRightWheel.WheelClass = UPhysXRearVehicleWheel::StaticClass();
	RearRightWheel.BoneName = "Rear_Right_Wheel";
	RearRightWheel.bDisableSteering = true;

	Vehicle->ComputeConstants();


#if WITH_EDITOR

	GetVertexPositions();
	Vehicle->DragArea = ComputeDragArea(Vertices);

	CentreOfMass = CreateDefaultSubobject<USceneComponent>("CentreOfMass");
	CentreOfMass->AttachTo(GetMesh());
	CentreOfMass->SetWorldLocation(GetMesh()->GetCenterOfMass());
	
#endif
}

void APhysXVehicle::BeginPlay()
{
	Super::BeginPlay();
}

void APhysXVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysXVehicle::SetMaxRPM(float inMaxRPM)
{
	Vehicle->MaxEngineRPM = inMaxRPM;
}

void APhysXVehicle::SetThrottle(float inThrottle)
{
	GetVehicleMovement()->SetThrottleInput(inThrottle);
}

void APhysXVehicle::SetTurn(float inTurn)
{
	GetVehicleMovement()->SetSteeringInput(inTurn);
}

#if WITH_EDITOR

bool APhysXVehicle::GetVertexPositions()
{
	MeshModel = GetMesh()->SkeletalMesh->GetImportedModel();
	MeshModel->LODModels[0].GetVertices(Vertices);


	if (Vertices.Num() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float APhysXVehicle::ComputeDragArea(TArray<FSoftSkinVertex> inVertices)
{
	//Skeletal Mesh cross sectional area aligned along the X axis so use Z and Y to calculate the Drag Area

	float MinY = 0;
	float MaxY = 0;

	float MinZ = 0;
	float MaxZ = 0;

	for (FSoftSkinVertex Vertex : inVertices)
	{
		if (Vertex.Position.Y > MaxY)
		{
			MaxY = Vertex.Position.Y;
		}
		if (Vertex.Position.Z > MaxZ)
		{
			MaxZ = Vertex.Position.Z;
		}

		if (Vertex.Position.Y < MinY)
		{
			MinY = Vertex.Position.Y;
		}
		if (Vertex.Position.Z < MinZ)
		{
			MinZ = Vertex.Position.Z;
		}
	}
	return ((MaxZ - MinZ) * (MaxY - MinY))/10000;
}

#endif

float APhysXVehicle::ComputeForwardVelocity()
{	
	//returns the velocity in the forward direction by getting the dot product
	//of the forward unit vector and the velocity vector/100 to return m/s
	return FVector::DotProduct((GetVelocity()/100), GetActorForwardVector());
}

float APhysXVehicle::ComputeDragForce()
{
	float Cd = Vehicle->DragCoefficient;
	float A = Vehicle->DragArea;
	float rho = 1.225;
	float V = ComputeForwardVelocity();

	float DragForce = (Cd * rho * V * V * A) / 2;

	return DragForce;
}

//Calls custom function I edited in the source code to change data at runtime
void APhysXVehicle::SetSuspensionData(float inSuspensionDampingRatio, float inMaxCompression, float inMaxDrop, float inNaturalFrequency)
{
	Vehicle->SetSuspensionData(inSuspensionDampingRatio, inMaxCompression, inMaxDrop, inNaturalFrequency);
}

void APhysXVehicle::SetWheelData(float inRadius, float inWidth, float inMaxSteerAngle, float inMass, float inDampingRate)
{
	Vehicle->SetWheelData(inRadius, inWidth, inMaxSteerAngle, inMass, inDampingRate);
}

float APhysXVehicle::GetSuspensionOffset(int32 inWheelIndex)
{
	return Vehicle->GetSuspensionOffset(inWheelIndex);
}

float APhysXVehicle::GetWheelMaxSuspensionCompression(int32 inWheelIndex)
{
	return Vehicle->GetMaxSuspensionCompression(inWheelIndex);
}

float APhysXVehicle::GetMaxSuspensionCompression()
{
	float MaxCompression = 0;
	for (int32 i = 0; i < Vehicle->WheelSetups.Num(); i++)
	{
		if (GetWheelMaxSuspensionCompression(i) > MaxCompression)
		{
			MaxCompression = GetWheelMaxSuspensionCompression(i);
		}
	}
	return MaxCompression;
}

bool APhysXVehicle::ServerSwitchToCharacter_Validate(APlayerController* inPlayerController, APresentationFinalCharacter* inCharacter)
{
	return true;
}

void APhysXVehicle::ServerSwitchToCharacter_Implementation(APlayerController* inPlayerController, APresentationFinalCharacter* inCharacter)
{
	if (PlayerCharacter)
	{
		inPlayerController->Possess(PlayerCharacter);
	}
	else
	{
		inPlayerController->Possess(inCharacter);
	}
}

void APhysXVehicle::SetPlayerCharacter(APresentationFinalCharacter* inPlayerCharacter)
{
	PlayerCharacter = inPlayerCharacter;
}

APresentationFinalCharacter* APhysXVehicle::GetPlayerCharacter()
{
	return PlayerCharacter;
}


PRAGMA_ENABLE_DEPRECATION_WARNINGS