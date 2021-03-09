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

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Replicated)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CylinderMesh;

	UPROPERTY(EditAnywhere, Replicated)
	UMaterial* BoardMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* BoardMaterialMI;

	UPROPERTY(EditAnywhere, Replicated)
	UMaterial* WhiteboardPen;

	UPROPERTY(EditAnywhere)
	class UCanvasRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* Widget;

	UPROPERTY()
	bool bOverlap;

	UPROPERTY(Replicated)
	float DrawSize;

	UPROPERTY(Replicated)
	float BoardOpacity;

	UPROPERTY(Replicated)
	bool bInvertColour;

	UPROPERTY(Replicated)
	FLinearColor XYCoords;
	
	//static TSharedRef<SRuntimeBoardEditorWidget, ESPMode::NotThreadSafe> BoardEditorWidgetRef;

	//TSharedPtr<SRuntimeBoardEditorWidget> BoardEditorWidgetPtr;

	//SRuntimeBoardEditorWidget* BoardEditorWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WhiteboardEditorWidgetClass = nullptr;


	UPROPERTY()
	class UMyUserWidget* WhiteboardEditorWidget;

	UPROPERTY()
	TArray<FLinearColor> Colors;

	//
	//UPROPERTY(Replicated)
	//UTexture2DDynamic* DynamicTexture;


	UPROPERTY()
	UMaterialInstanceDynamic* PenMI;

	UFUNCTION(BlueprintCallable)
	void Paint(FVector2D PaintLocation);


	//UFUNCTION(BlueprintCallable)
	//void SetDrawSize(float DrawSize);

	UFUNCTION()
	void OnRep_SetUpBoard();

	UFUNCTION()
	void SetBoardOpacity(float inOpacity);

	UFUNCTION()
	void SetDrawSize(float inDrawSize);

	//client and netmulticast work
	//UFUNCTION(NetMultiCast, Reliable, WithValidation)
	//void Multi_OnRep_SetInvertColour(bool InbInvertColour);
	//bool Multi_OnRep_SetInvertColour_Validate(bool InbInvertColour);
	//void Multi_OnRep_SetInvertColour_Implementation(bool InbInvertColour);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_OnRep_SetInvertColour(bool InbInvertColour);
	bool Client_OnRep_SetInvertColour_Validate(bool InbInvertColour);
	void Client_OnRep_SetInvertColour_Implementation(bool InbInvertColour);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UFUNCTION(NetMulticast, Reliable, WithValidation)
	//void Server_OnSetInvert(bool InInvertColour);
	//bool Server_OnSetInvert_Validate(bool InInvertColour);
	//void Server_OnSetInvert_Implementation(bool InInvertColour);

	//UFUNCTION(Server, Reliable, WithValidation)
	//void Client_OnSetInvert(bool InInvertColour);
	//bool Client_OnSetInvert_Validate(bool InInvertColour);
	//void Client_OnSetInvert_Implementation(bool InInvertColour);

	UFUNCTION()
	void OnRep_OnSetInvert();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
