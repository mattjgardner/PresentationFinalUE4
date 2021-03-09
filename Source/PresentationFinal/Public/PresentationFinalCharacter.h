// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PhysXVehicle.h"
#include "PresentationFinalCharacter.generated.h"


USTRUCT(BlueprintType)
struct FVehicleControllerStruct
{
	GENERATED_BODY();

public:
	UPROPERTY()
		TArray<APlayerController*> MapKeys;

	UPROPERTY()
		TArray<APhysXVehicle*> MapValues;

		FVehicleControllerStruct(){}

		void AddKey(APlayerController* inKey)
		{
			MapKeys.Add(inKey);
		}

		void AddValue(APhysXVehicle* inValue)
		{
			MapValues.Add(inValue);
		}

		APhysXVehicle* GetValueFromKey(APlayerController* inKey)
		{
			APhysXVehicle* Vehicle = nullptr;
			for (int32 i = 0; i < MapKeys.Num(); i++)
			{
				if (inKey->GetName() == MapKeys[i]->GetName())
				{
					UE_LOG(LogTemp, Log, TEXT("working"));
					Vehicle = MapValues[i];
				}
			}
			return Vehicle;
		}
};

UCLASS(config=Game)
class APresentationFinalCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

	///** Follow camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
		class AVirtualPresentationHUD* HUD;

	UPROPERTY()
		class UMyUserWidget* WhiteboardEditorWidget;

	UPROPERTY(EditAnywhere)
		class USlideControllerWidget* SlideControllerWidget;

	UPROPERTY()
		class APlayerController* PlayerController;

	//UPROPERTY(BlueprintReadWrite)
		//class APresentationFinalCharacter* PlayerCharacter;

	//UPROPERTY(BlueprintReadWrite)
		class APhysXVehicle* ControlledVehicle;

	UPROPERTY()
		class ABoardActor* BoardActor;

	UPROPERTY()
		class ASlideActor* SlideActor;

	UPROPERTY()
		bool bWhiteboardOverlap;

	UPROPERTY()
		bool bSlideOverlap;

	UPROPERTY()
		bool bDraw;

	UPROPERTY(ReplicatedUsing=OnRep_Invert)
		bool bInvertColour;

	UPROPERTY(ReplicatedUsing=OnRep_BoardOpacity)
		float BoardOpacity;

	UPROPERTY(ReplicatedUsing=OnRep_UVHitLocation)
		FVector2D UVHitPosition;

	UPROPERTY(ReplicatedUsing=OnRep_DrawSize)
		float MarkerDrawSize;

	UPROPERTY(ReplicatedUsing=OnRep_StartSlideshow);
	bool bStartSlideshow;


	UPROPERTY(ReplicatedUsing = OnRep_PauseSlideshow);
	bool bPauseSlideshow;

	UPROPERTY(Replicated)
	FVehicleControllerStruct VehicleControllerStruct;

	UPROPERTY(ReplicatedUsing=OnRep_VehicleStruct)
		TArray<APlayerController*> ControllerMapKeys;

	UPROPERTY(Replicated=OnRep_VehicleStruct)
		TArray<APhysXVehicle*> VehicleMapValues;


	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void SwitchInputMode();

	UFUNCTION()
		void ZoomIn();

	UFUNCTION()
		void ZoomOut();

	UFUNCTION()
		FVector2D Draw();

	UFUNCTION()
		void SetbDrawTrue();

	UFUNCTION()
		void SetbDrawFalse();

	UFUNCTION()
		void OnTickWhiteboardControlCheck();

	UFUNCTION()
		void OnTickSlideControlCheck();

	UFUNCTION(BlueprintCallable)
		APhysXVehicle* GetPhysXVehicleFromController(APlayerController* inController);



	UFUNCTION()
		void OnRep_Invert();

		void UpdateInvert();



	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdateInvert(bool InbInvert);
		bool ServerUpdateInvert_Validate(bool InbInvert);
		void ServerUpdateInvert_Implementation(bool InbInvert);




	UFUNCTION()
		void OnRep_BoardOpacity();

		void UpdateBoardOpacity();



	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdateOpacity(float inOpacity);
		bool ServerUpdateOpacity_Validate(float inOpacity);
		void ServerUpdateOpacity_Implementation(float InOpacity);


	UFUNCTION()
		void OnRep_UVHitLocation();

		void UpdateUVHitLocation();



	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerHitLocation(FVector2D HitLocation);
		bool ServerHitLocation_Validate(FVector2D HitLocation);
		void ServerHitLocation_Implementation(FVector2D HitLocation);




		UFUNCTION(Server, Reliable, WithValidation)
			void ServerStartSlideshow(bool InbStartShow);
		bool ServerStartSlideshow_Validate(bool InbStartShow);
		void ServerStartSlideshow_Implementation(bool InbStartShow);

		UFUNCTION()
			void OnRep_StartSlideshow();

		void UpdateStartSlideshow();


		UFUNCTION(Server, Reliable, WithValidation)
			void ServerPauseSlideshow(bool InbPauseShow);
		bool ServerPauseSlideshow_Validate(bool InbPauseShow);
		void ServerPauseSlideshow_Implementation(bool InbPauseShow);


		UFUNCTION()
			void OnRep_PauseSlideshow();

		void UpdatePauseSlideshow();


		//UFUNCTION(Server, Reliable, WithValidation)
		//	void ServerUnPauseSlideshow(bool InbPauseShow);
		//bool ServerUnPauseSlideshow_Validate(bool InbPauseShow);
		//void ServerUnPauseSlideshow_Implementation(bool InbPauseShow);

		//UFUNCTION()
		//	void OnRep_UnPauseSlideshow();

		//void UpdateUnPauseSlideshow();

		//UFUNCTION(Server, Reliable, WithValidation)
		//	void ServerPauseSlideshow(bool InbPauseShow);
		//bool ServerPauseSlideshow_Validate(bool InbPauseShow);
		//void ServerPauseSlideshow_Implementation(bool InbPauseShow);

		//void OnRep_PauseSlideShow();

		//void UpdatePauseSlideShow();

//if replication for draw size is required
	UFUNCTION()
		void OnRep_DrawSize();

	void UpdateDrawSize();

	UFUNCTION()
		void OnRep_VehicleStruct();

	void UpdateVehicleStruct();

	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdateDrawSize(float inDrawSize);
		bool ServerUpdateDrawSize_Validate(float inDrawSize);
		void ServerUpdateDrawSize_Implementation(float InDrawSize);

		UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
			void ServerSpawnAndPossessPawn(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);
		bool ServerSpawnAndPossessPawn_Validate(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);
		void ServerSpawnAndPossessPawn_Implementation(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);

		UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
			void ServerSwitchPossessedPawn(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);
		bool ServerSwitchPossessedPawn_Validate(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);
		void ServerSwitchPossessedPawn_Implementation(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);

		UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
			void ServerSpawnPawn(TSubclassOf<class APawn> inPawn);
		bool ServerSpawnPawn_Validate(TSubclassOf<class APawn> inPawn);
		void ServerSpawnPawn_Implementation(TSubclassOf<class APawn> inPawn);

		//UFUNCTION(BlueprintCallable)

		//UFUNCTION(BlueprintCallable)
		//	void SwitchPossessedPawn(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn);



public:
	APresentationFinalCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
public:
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	////** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

