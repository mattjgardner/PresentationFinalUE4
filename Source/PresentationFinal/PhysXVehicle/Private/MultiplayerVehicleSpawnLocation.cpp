// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerVehicleSpawnLocation.h"
#include "Components/SceneComponent.h"

// Sets default values
AMultiplayerVehicleSpawnLocation::AMultiplayerVehicleSpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>("Location");

}

// Called when the game starts or when spawned
void AMultiplayerVehicleSpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMultiplayerVehicleSpawnLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AMultiplayerVehicleSpawnLocation::GetLocation()
{
	return LocationComponent->GetComponentLocation();
}

