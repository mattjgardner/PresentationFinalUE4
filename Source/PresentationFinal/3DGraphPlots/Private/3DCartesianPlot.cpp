// Fill out your copyright notice in the Description page of Project Settings.


#include "3DCartesianPlot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "CartesianPlotPoint.h"



// Sets default values
A3DCartesianPlot::A3DCartesianPlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	

	AxisSize = 100;
	ArrowSize = 10;
	
	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;
	Origin->SetWorldScale3D(FVector(0.1, 0.1, 0.1));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->AttachTo(RootComponent);

	XAxis = CreateDefaultSubobject<UArrowComponent>("XAxis");
	XAxis->SetupAttachment(Origin, NAME_None);
	XAxis->SetArrowColor(FLinearColor::Red);
	XAxis->ArrowLength = AxisSize;
	XAxis->ArrowSize = ArrowSize;
	XAxis->AddLocalRotation(FRotator(0, 0, 90));
	XAxis->SetVisibility(true);
	XAxis->SetHiddenInGame(false);

	YAxis = CreateDefaultSubobject<UArrowComponent>("YAxis");
	YAxis->SetupAttachment(Origin, NAME_None);
	YAxis->SetArrowColor(FLinearColor::Blue);
	YAxis->ArrowLength = AxisSize;
	YAxis->ArrowSize = ArrowSize;
	YAxis->AddLocalRotation(FRotator(90, 0, 0));
	YAxis->SetVisibility(true);
	YAxis->SetHiddenInGame(false);

	ZAxis = CreateDefaultSubobject<UArrowComponent>("ZAxis");
	ZAxis->SetupAttachment(Origin, NAME_None);
	ZAxis->SetArrowColor(FLinearColor::Green);
	ZAxis->ArrowLength = AxisSize;
	ZAxis->ArrowSize = ArrowSize;
	ZAxis->AddLocalRotation(FRotator(0, 90, 0));
	ZAxis->SetVisibility(true);
	ZAxis->SetHiddenInGame(false);

}

// Called when the game starts or when spawned
void A3DCartesianPlot::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < 100; i++)
	{
		FVector RandomVector = FMath::VRand();
		XData.Add(RandomVector.X);
		YData.Add(RandomVector.Y);
		ZData.Add(RandomVector.Z);
	}

	XData.Add(1);
	YData.Add(0);
	ZData.Add(0);

	XData.Add(0);
	YData.Add(1);
	ZData.Add(0);

	XData.Add(0);
	YData.Add(0);
	ZData.Add(1);

	ScaleAxis(XData, XDataRelative);
	ScaleAxis(YData, YDataRelative);
	ScaleAxis(ZData, ZDataRelative);

	PlotData(XDataRelative, YDataRelative, ZDataRelative);
}

// Called every frame
void A3DCartesianPlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void A3DCartesianPlot::ScaleAxis(TArray<float> inData, TArray<float> &RelativeValues)
{
	float Max = 0;
	for (float Coordinate : inData)
	{
		if (Coordinate > Max)
		{
			Max = Coordinate;
		}
	}
	if (Max != 0)
	{
		for (float Coordinate : inData)
		{
			RelativeValues.Add(Coordinate / Max);
		}
	}
}

void A3DCartesianPlot::PlotData(TArray<float> ScaledXAxis, TArray<float> ScaledYAxis, TArray<float> ScaledZAxis)
{
	FVector OriginPosition = Origin->GetComponentLocation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (float index : ScaledXAxis)
	{
		FVector SpawnPosition = OriginPosition + FVector(ScaledXAxis[index], ScaledYAxis[index], -ScaledZAxis[index]) * AxisSize;
		UWorld* World = GetWorld();

		if (World)
		{
			//World->SpawnActor<ACartesianPlotPoint>(SpawnPosition, FRotator(0,0,0), SpawnParams);
			//UE_LOG(LogTemp, Log, TEXT("%s"), *SpawnPosition.ToString());
		}
	
	}
}

