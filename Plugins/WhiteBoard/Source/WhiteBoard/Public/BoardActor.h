// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardActor.generated.h"

UCLASS()
class WHITEBOARD_API ABoardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardActor();

	//Create box to collide with so when collision, user pops UI for controlling the whiteboard
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	
	//Mesh for the whiteboard, is set to a cube
	UPROPERTY(EditAnywhere, Replicated)
	UStaticMeshComponent* Mesh;

	//Material of the board
	UPROPERTY(EditAnywhere, Replicated)
	UMaterial* BoardMaterial;

	//Material instance of the board material
	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* BoardMaterialMI;

	//Material for the Pen
	UPROPERTY(EditAnywhere, Replicated)
	UMaterial* WhiteboardPen;

	//Material Instance for the pen
	UPROPERTY()
	UMaterialInstanceDynamic* PenMI;

	//Render target created when board actor is spawned
	//this means all whiteboards can have different writing on
	//as a shared render target means they all display the same thing
	UPROPERTY(EditAnywhere)
	class UCanvasRenderTarget2D* RenderTarget;

	//replicated
	UPROPERTY()
	float DrawSize;

	UPROPERTY()
	float BoardOpacity;

	UPROPERTY()
	bool bInvertColour;

	UPROPERTY()
	FLinearColor XYCoords;

	UPROPERTY()
	bool bReadyToDraw;


	UFUNCTION(BlueprintCallable)
	void Paint(FVector2D PaintLocation);

	UFUNCTION()
	void OnRep_SetUpBoard();

	UFUNCTION()
	void SetBoardOpacity(float inOpacity);

	UFUNCTION()
	void SetDrawSize(float inDrawSize);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_OnRep_SetInvertColour(bool InbInvertColour);
	bool Client_OnRep_SetInvertColour_Validate(bool InbInvertColour);
	void Client_OnRep_SetInvertColour_Implementation(bool InbInvertColour);

	UFUNCTION()
	void ClearBoard();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_OnSetInvert();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
