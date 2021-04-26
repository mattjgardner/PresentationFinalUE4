// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlideActor.generated.h"

UCLASS()
class PRESENTATIONFINAL_API ASlideActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlideActor();

	//All properties exposed to editor so users can change the values in the editor.
	//E.g. the slides, slide sounds to have a custom sound not defined by the 
	//python import folder




	//Anything that's not a Path and the mesh should be set in the editor to prevent crashes in the packaged build

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GameDirectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MaterialsDirectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SoundsDirectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString slide1Directory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> MaterialPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> SoundPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* SlideSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<USoundBase*> SlideSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMaterial*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SlideIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;


	UPROPERTY()
		float SlideTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> SlideTimes;

	UPROPERTY()
		FTimerHandle SlideTimerHandle;

	UPROPERTY()
		bool bPaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsUsingOVRLipsync;

	UFUNCTION()
		void StartSlideshow();

	UFUNCTION()
		void PauseUnpauseSlideshow();

	UFUNCTION()
		void SetSlide(int32 inSlideIndex, TArray<UMaterial*> inMaterials);

	UFUNCTION(BlueprintCallable)
		int32 GetSlideIndex();

	UFUNCTION(BlueprintCallable)
		void SetSlideIndex(int32 inSlideIndex);

	UFUNCTION(BlueprintCallable)
		bool GetBoolIsStarted();

	UFUNCTION(BlueprintCallable)
		bool GetBoolIsPaused();

	UFUNCTION(BlueprintCallable)
		void SetBoolIsUsingOVRLipSync();

	UFUNCTION(BlueprintCallable)
		void GetSoundData(TArray<float> &inSlideTimes, TArray<USoundBase*> &inSlideSounds);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
