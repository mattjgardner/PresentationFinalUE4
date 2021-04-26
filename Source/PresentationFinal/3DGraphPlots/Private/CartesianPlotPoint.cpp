// Fill out your copyright notice in the Description page of Project Settings.


#include "CartesianPlotPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACartesianPlotPoint::ACartesianPlotPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creates a point for a 3D graph

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("GraphPoint");
	UStaticMesh* SphereMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object;
	Mesh->SetStaticMesh(SphereMesh);
	Mesh->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
	RootComponent = Mesh;

	//Material->Colour

	Material = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Engine/EngineMaterials/EmissiveMeshMaterial.EmissiveMeshMaterial'")).Object;
	Mesh->SetMaterial(0, Material);
}

// Called when the game starts or when spawned
void ACartesianPlotPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (Material)
	{
		Mesh->SetMaterial(0, Material);
	}

	Mesh->SetHiddenInGame(false);

}

// Called every frame
void ACartesianPlotPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

