// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "3DCartesianPlot.generated.h"

UCLASS()
class PRESENTATIONFINAL_API A3DCartesianPlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	A3DCartesianPlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* YAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* ZAxis;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AxisSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ArrowSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* WireFrameMaterial;

	//TMap<float, float> Data;

	TArray<float> XData;
	TArray<float> YData;
	TArray<float> ZData;

	TArray<float> XDataRelative;
	TArray<float> YDataRelative;
	TArray<float> ZDataRelative;


	UFUNCTION(BlueprintCallable)
		void ScaleAxis(TArray<float> inData, TArray<float> &RelativeValues);


	UFUNCTION(BlueprintCallable)
		void PlotData(TArray<float> inXData, TArray<float> inYData, TArray<float> inZData);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};