// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent.h"
#include "Rendering/SkeletalMeshModel.h"
#include "VehicleWheel.h"
#include "PhysXVehicle.generated.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS



UCLASS()
class PRESENTATIONFINAL_API APhysXVehicle : public AWheeledVehicle
{

	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		USkeletalMesh* SportsCarSKMesh;

	UPROPERTY(BlueprintReadWrite)
		USceneComponent* CentreOfMass;

	UPROPERTY(BlueprintReadWrite)
		TMap<float, float> RPMTorque;

	UPROPERTY(BlueprintReadWrite)
	class UWheeledVehicleMovementComponent4W* Vehicle;

	UPROPERTY(BlueprintReadWrite)
	class UPhysXFrontVehicleWheel* FrontLeftWheelP;


	//UPROPERTY(BlueprintReadWrite)
	//	FWheelSetup FrontLeftWheel;


	//UPROPERTY(BlueprintReadWrite)
	//	FWheelSetup FrontRightWheel;


	//UPROPERTY(BlueprintReadWrite)
	//	FWheelSetup RearLeftWheel;


	//UPROPERTY(BlueprintReadWrite)
	//	FWheelSetup RearRightWheel;


	FSkeletalMeshModel* MeshModel;

#if WITH_EDITOR
	TArray<FSoftSkinVertex> Vertices;
#endif


	//UPROPERTY()
	//	FWheelSetup FrontLeftWheel;

	UFUNCTION(BlueprintCallable)
	void SetMaxRPM(float inMaxRPM);

	UFUNCTION(BlueprintCallable)
		void SetThrottle(float inThrottle);

	UFUNCTION(BlueprintCallable)
		void SetTurn(float inTurn);

#if WITH_EDITOR

	UFUNCTION(BlueprintCallable)
		bool GetVertexPositions();

#endif

	UFUNCTION(BlueprintCallable)
		float ComputeForwardVelocity();

	UFUNCTION(BlueprintCallable)
		float ComputeDragForce();

	UFUNCTION(BlueprintCallable)
		void SetSuspensionData(float inSuspensionDampingRatio, float inMaxCompression, float inMaxDrop, float inNaturalFrequency);

	UFUNCTION(BlueprintCallable)
		void SetWheelData(float inRadius, float inWidth, float inMaxSteerAngle, float inMass, float inDampingRate);

	UFUNCTION(BlueprintCallable)
		float GetSuspensionOffset(int32 inWheelIndex);

	UFUNCTION(BlueprintCallable)
		float GetWheelMaxSuspensionCompression(int32 inWheleIndex);

	UFUNCTION(BlueprintCallable)
		float GetMaxSuspensionCompression();


	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void ServerSwitchToCharacter(APlayerController* inPlayerController, APresentationFinalCharacter* inCharacter);
	bool ServerSwitchToCharacter_Validate(APlayerController* inPlayerController, APresentationFinalCharacter* inCharacter);
	void ServerSwitchToCharacter_Implementation(APlayerController* inPlayerController, APresentationFinalCharacter* inCharacter);

#if WITH_EDITOR

	float ComputeDragArea(TArray<FSoftSkinVertex> inVertices);

#endif
	
public:
	APhysXVehicle();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};



//USTRUCT(BlueprintType)
//struct FRuntimeFloatCurve
//{
//	GENERATED_BODY();
//
//
//};
//
//
//
//USTRUCT(BlueprintType)
//struct FVehicleDifferential4WData
//{
//	GENERATED_BODY();
//
//
//
//};
//
//USTRUCT(BlueprintType)
//struct FVehicleEngineData
//{
//	GENERATED_BODY();
//
//
//
//
//};
//
//
//USTRUCT(BlueprintType)
//struct FVehicleTransmissionSetup
//{
//	GENERATED_BODY();
//
//
//};

PRAGMA_ENABLE_DEPRECATION_WARNINGS