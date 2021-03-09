// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayerVehicleSpawnLocation.generated.h"

UCLASS()
class PRESENTATIONFINAL_API AMultiplayerVehicleSpawnLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultiplayerVehicleSpawnLocation();

	UPROPERTY(BlueprintReadWrite)
		USceneComponent* LocationComponent;

	UFUNCTION(BlueprintCallable)
		FVector GetLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
