// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CartesianPlotPoint.generated.h"

UCLASS()
class PRESENTATIONFINAL_API ACartesianPlotPoint : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ACartesianPlotPoint();


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* GraphPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* Material;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
