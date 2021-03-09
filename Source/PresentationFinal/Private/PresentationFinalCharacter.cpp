// Copyright Epic Games, Inc. All Rights Reserved.

#include "PresentationFinalCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include <WhiteBoard/Public/MyUserWidget.h>
#include <WhiteBoard/Public/BoardActor.h>
#include "SlideControllerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "VirtualPresentationHUD.h"
#include "SlideActor.h"
#include "PresentationFinalGameMode.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerVehicleSpawnLocation.h"


//////////////////////////////////////////////////////////////////////////
// APresentationFinalCharacter

APresentationFinalCharacter::APresentationFinalCharacter()
{
	bReplicates = true;

	//UGameplayStatics::OpenLevel();
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(19.f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APresentationFinalCharacter::BeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APresentationFinalCharacter::EndOverlap);

}

void APresentationFinalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*if (bDraw == true)
	{
		Draw();
	}*/

	OnTickWhiteboardControlCheck();
	
	OnTickSlideControlCheck();

}

void APresentationFinalCharacter::OnTickWhiteboardControlCheck()
{
	if (bWhiteboardOverlap == true)
	{
		if (WhiteboardEditorWidget)
		{
			if (PlayerController)
			{
				if (PlayerController->bShowMouseCursor == true)
				{
					if (BoardActor)
					{
						if (IsLocallyControlled())
						{
							if (WhiteboardEditorWidget->GetBoolInvert() == false)
							{
								ServerUpdateInvert(false);
							}
							if (WhiteboardEditorWidget->GetBoolInvert() == true)
							{
								if (GetLocalRole() < ROLE_Authority)
								{
									ServerUpdateInvert(true);

								}
								else
								{
									UpdateInvert();
								}
								WhiteboardEditorWidget->SetBoolInvert();
							}

							if (GetLocalRole() < ROLE_Authority)
							{
								ServerUpdateOpacity(WhiteboardEditorWidget->GetWhiteboardOpacity());
								MarkerDrawSize = WhiteboardEditorWidget->GetDrawSize();
								UpdateDrawSize();
							}
							else
							{
								UpdateBoardOpacity();
							}

							if (GetLocalRole() < ROLE_Authority)
							{
								//Only sets local user's draw size
								//As a result, not all user's see the same size marker
								//Local user sees large circle, other clients see small marker still
								this->BoardActor->SetDrawSize(WhiteboardEditorWidget->GetDrawSize());
							}


						}
					}
					/*MarkerDrawSize = WhiteboardEditorWidget->GetDrawSize();
					BoardOpacity = WhiteboardEditorWidget->GetWhiteboardOpacity();*/
				}

				if (bDraw == true)
				{
					if (GetLocalRole() < ROLE_Authority)
					{
						//UVHitPosition = Draw();
						ServerHitLocation(Draw());
						if (WhiteboardEditorWidget->GetDrawSize() > 0.01f)
						{
							MarkerDrawSize = WhiteboardEditorWidget->GetDrawSize();
						}
						else
						{
							MarkerDrawSize = 0.01f;
						}
					}
					else
					{
						UpdateUVHitLocation();
					}

				}



			}
		}
	}
}

void APresentationFinalCharacter::OnTickSlideControlCheck()
{
	if (bSlideOverlap == true)
	{
		if (SlideControllerWidget)
		{
			if (PlayerController)
			{
				if (PlayerController->bShowMouseCursor == true)
				{
					if (SlideActor)
					{
						if (IsLocallyControlled())
						{
							if (SlideControllerWidget->GetBoolStarted() == true)
							{
								if (GetLocalRole() < ROLE_Authority)
								{
									SlideControllerWidget->bIsStarted = false;
									ServerStartSlideshow(true);
								}
							}
							if (GetLocalRole() < ROLE_Authority)
							{
								if ((SlideControllerWidget->GetPause() % 2) != 0)
								{
									ServerPauseSlideshow(true);
								}
								else
								{
									ServerPauseSlideshow(false);
								}

							}
						}
					}
				}
			}
		}
	}
}

void APresentationFinalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APresentationFinalCharacter, bInvertColour, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(APresentationFinalCharacter, BoardOpacity);
	DOREPLIFETIME(APresentationFinalCharacter, UVHitPosition);
	DOREPLIFETIME(APresentationFinalCharacter, MarkerDrawSize);
	DOREPLIFETIME(APresentationFinalCharacter, bStartSlideshow);
	DOREPLIFETIME(APresentationFinalCharacter, bPauseSlideshow);
	DOREPLIFETIME(APresentationFinalCharacter, VehicleControllerStruct);
	DOREPLIFETIME(APresentationFinalCharacter, ControllerMapKeys);
	DOREPLIFETIME(APresentationFinalCharacter, VehicleMapValues);
}




//////////////////////////////////////////////////////////////////////////
// Input

void APresentationFinalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APresentationFinalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APresentationFinalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APresentationFinalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APresentationFinalCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &APresentationFinalCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APresentationFinalCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APresentationFinalCharacter::OnResetVR);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &APresentationFinalCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &APresentationFinalCharacter::ZoomOut);

	PlayerInputComponent->BindAction("InputSwitch", IE_Pressed, this, &APresentationFinalCharacter::SwitchInputMode);

	PlayerInputComponent->BindAction("Draw", IE_Pressed, this, &APresentationFinalCharacter::SetbDrawTrue);
	PlayerInputComponent->BindAction("Draw", IE_Released, this, &APresentationFinalCharacter::SetbDrawFalse);
}


void APresentationFinalCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APresentationFinalCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void APresentationFinalCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void APresentationFinalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APresentationFinalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APresentationFinalCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APresentationFinalCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APresentationFinalCharacter::BeginOverlap(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->GetName() == "BoardActor")
	{
		BoardActor = Cast<ABoardActor>(OtherActor);
		if (IsLocallyControlled())
		{
			PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				HUD = Cast<AVirtualPresentationHUD>(PlayerController->GetHUD());;
				if (HUD)
				{
					if (HUD->bAddedToViewport == true)
					{
						HUD->RemoveWhiteboardEditorWidget();
					}
					HUD->AddWhiteboardEditorWidget();
					if (bWhiteboardOverlap == false)
					{
						WhiteboardEditorWidget = (HUD->GetWhiteboardEditorWidget());
						bWhiteboardOverlap = true;
					}
				}
			}

		}

	}
	else if (OtherActor->GetClass()->GetName() == "SlideActor")
	{
		SlideActor = Cast<ASlideActor>(OtherActor);
		if (IsLocallyControlled())
		{
			PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				HUD = Cast<AVirtualPresentationHUD>(PlayerController->GetHUD());
				if (HUD)
				{
					if (HUD->bAddedSlideWidgetToViewport == true)
					{
						HUD->RemoveSlideControllerWidget();
					}
					HUD->AddSlideControllerWidget();
					if (bSlideOverlap == false)
					{
						SlideControllerWidget = (HUD->GetSlideControllerWidget());
						bSlideOverlap = true;

					}
				}

			}

		}
	}
}

void APresentationFinalCharacter::EndOverlap(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex)
{
	if (HUD)
	{
		if (WhiteboardEditorWidget)
		{
			HUD->RemoveWhiteboardEditorWidget();
		}
		if (SlideControllerWidget)
		{
			HUD->RemoveSlideControllerWidget();
		}
	}
	bWhiteboardOverlap = false;
}

void APresentationFinalCharacter::SwitchInputMode()
{
	if (PlayerController)
	{
		if (bWhiteboardOverlap == true && PlayerController->bShowMouseCursor == false)
		{
			FInputModeGameAndUI InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;
		}
		else if(bSlideOverlap == true && PlayerController->bShowMouseCursor == false)
		{
			FInputModeGameAndUI InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;
		}
		else
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = false;
		}
	}
}

void APresentationFinalCharacter::ZoomIn()
{
	FirstPersonCameraComponent->SetFieldOfView((FirstPersonCameraComponent->FieldOfView) * 0.75);
}

void APresentationFinalCharacter::ZoomOut()
{
	if (FirstPersonCameraComponent->FieldOfView / 0.75 <= 90)
	{
		FirstPersonCameraComponent->SetFieldOfView((FirstPersonCameraComponent->FieldOfView) / 0.75);
	}
	else
	{
		FirstPersonCameraComponent->SetFieldOfView(90);
	}
	
}

void APresentationFinalCharacter::SetbDrawTrue()
{
	bDraw = true;
}

void APresentationFinalCharacter::SetbDrawFalse()
{
	bDraw = false;
}


FVector2D APresentationFinalCharacter::Draw()
{
	FVector2D UVHit;
	if (bWhiteboardOverlap == true)
	{
		FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector ForwardVector = 1000 * FirstPersonCameraComponent->GetForwardVector();

		TArray<AActor*> Actors;
		FHitResult HitResult;
		UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, (CameraLocation + ForwardVector), ETraceTypeQuery::TraceTypeQuery1, true, Actors, EDrawDebugTrace::None, HitResult, true);

		AActor* HitActor = HitResult.GetActor();

		if (HitActor != nullptr)
		{
			if (HitActor->GetClass()->GetName() == "BoardActor")
			{
				BoardActor = Cast<ABoardActor>(HitActor);
				UGameplayStatics::FindCollisionUV(HitResult, 0, UVHit);
				//BoardActor->Paint(UVHit);

				//UE_LOG(LogTemp, Log, TEXT("%s"), *UVHit.ToString());
				//UVHitPosition = UVHit;
			}
		}
	}
	return UVHit;
}

void APresentationFinalCharacter::OnRep_Invert()
{
	UpdateInvert();
}

void APresentationFinalCharacter::UpdateInvert()
{ 
	this->BoardActor->Client_OnRep_SetInvertColour(bInvertColour);
}

bool APresentationFinalCharacter::ServerUpdateInvert_Validate(bool InbInvert)
{
	return true;
}

void APresentationFinalCharacter::ServerUpdateInvert_Implementation(bool InbInvert)
{
	bInvertColour = InbInvert;
}




void APresentationFinalCharacter::OnRep_BoardOpacity()
{
	UpdateBoardOpacity();
}

void APresentationFinalCharacter::UpdateBoardOpacity()
{
	this->BoardActor->SetBoardOpacity(BoardOpacity);
}

bool APresentationFinalCharacter::ServerUpdateOpacity_Validate(float inOpacity)
{
	return true;
}

void APresentationFinalCharacter::ServerUpdateOpacity_Implementation(float inOpacity)
{
	BoardOpacity = inOpacity;
}

void APresentationFinalCharacter::OnRep_UVHitLocation()
{
	UpdateUVHitLocation();
}

void APresentationFinalCharacter::UpdateUVHitLocation()
{
	this->BoardActor->Paint(UVHitPosition);
}

bool APresentationFinalCharacter::ServerHitLocation_Validate(FVector2D HitLocation)
{
	return true;
}

void APresentationFinalCharacter::ServerHitLocation_Implementation(FVector2D HitLocation)
{
	UVHitPosition = HitLocation;
}




//In case replication for Marker Draw size is wanted
void APresentationFinalCharacter::OnRep_DrawSize()
{
	UpdateDrawSize();
}

void APresentationFinalCharacter::UpdateDrawSize()
{
	this->BoardActor->SetDrawSize(MarkerDrawSize);
}


bool APresentationFinalCharacter::ServerUpdateDrawSize_Validate(float inDrawSize)
{
	return true;
}

void APresentationFinalCharacter::ServerUpdateDrawSize_Implementation(float inDrawSize)
{
	MarkerDrawSize = inDrawSize; 
}


void APresentationFinalCharacter::OnRep_StartSlideshow()
{
	UpdateStartSlideshow();
}

void APresentationFinalCharacter::UpdateStartSlideshow()
{
	this->SlideActor->StartSlideshow();
}


bool APresentationFinalCharacter::ServerStartSlideshow_Validate(bool inbStartSlideshow)
{
	return true;
}

void APresentationFinalCharacter::ServerStartSlideshow_Implementation(bool inbStartSlideshow)
{
	bStartSlideshow = inbStartSlideshow;
}

void APresentationFinalCharacter::OnRep_PauseSlideshow()
{
	UpdatePauseSlideshow();
	UE_LOG(LogTemp, Log, TEXT("onrep"));
}

void APresentationFinalCharacter::UpdatePauseSlideshow()
{
	this->SlideActor->PauseUnpauseSlideshow();
}

bool APresentationFinalCharacter::ServerPauseSlideshow_Validate(bool inbPauseSlideshow)
{
	return true;
}

void APresentationFinalCharacter::ServerPauseSlideshow_Implementation(bool inbPauseSlideshow)
{
	bPauseSlideshow = inbPauseSlideshow;
}


bool APresentationFinalCharacter::ServerSpawnAndPossessPawn_Validate(APlayerController* inPlayerController, TSubclassOf<APawn> inPawn)
{
	return true;
}

void APresentationFinalCharacter::ServerSpawnAndPossessPawn_Implementation(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn)
{
	FVector SpawnLocation;
	AMultiplayerVehicleSpawnLocation* SpawnLocationActor = Cast<AMultiplayerVehicleSpawnLocation>(UGameplayStatics::GetActorOfClass(GetWorld(), AMultiplayerVehicleSpawnLocation::StaticClass()));

	if (SpawnLocationActor)
	{
		SpawnLocation = SpawnLocationActor->GetActorLocation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (ControlledVehicle)
		{
			inPlayerController->Possess(ControlledVehicle);
		}
		else
		{
			APawn* const SpawnedPawn = GetWorld()->SpawnActor<APawn>(inPawn->GetDefaultObject()->GetClass(), SpawnLocation, FRotator(0, 0, 0), SpawnParams);
			ControlledVehicle = Cast<APhysXVehicle>(SpawnedPawn);
			if (ControlledVehicle)
			{
				inPlayerController->Possess(ControlledVehicle);
			}
			//ControlledVehicle = Cast<TSubclassOf<class APawn>>(SpawnedPawn);
			//inPlayerController->Possess(SpawnedPawn);
		}

	}
}


bool APresentationFinalCharacter::ServerSwitchPossessedPawn_Validate(APlayerController* inPlayerController, TSubclassOf<APawn> inPawn)
{
	return true;
}

void APresentationFinalCharacter::ServerSwitchPossessedPawn_Implementation(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Made it to server switch pawn"));
	inPlayerController->UnPossess();
	if (ControlledVehicle)
	{
		inPlayerController->Possess(ControlledVehicle);
	}
	else
	{
		ServerSpawnPawn(inPawn);
		inPlayerController->Possess(ControlledVehicle);
	}
}

void APresentationFinalCharacter::OnRep_VehicleStruct()
{
	UpdateVehicleStruct();
}

void APresentationFinalCharacter::UpdateVehicleStruct()
{

}

bool APresentationFinalCharacter::ServerSpawnPawn_Validate(TSubclassOf<APawn> inPawn)
{
	return true;
}

void APresentationFinalCharacter::ServerSpawnPawn_Implementation(TSubclassOf<class APawn> inPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Made it to server spawn pawn"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMultiplayerVehicleSpawnLocation* SpawnLocationActor = Cast<AMultiplayerVehicleSpawnLocation>(UGameplayStatics::GetActorOfClass(GetWorld(), AMultiplayerVehicleSpawnLocation::StaticClass()));

	if(SpawnLocationActor)
	{
		APawn* const SpawnedPawn = GetWorld()->SpawnActor<APawn>(inPawn->GetDefaultObject()->GetClass(), SpawnLocationActor->LocationComponent->GetComponentLocation(), FRotator(0, 0, 0), SpawnParams);
		ControlledVehicle = Cast<APhysXVehicle>(SpawnedPawn);
	}
}

APhysXVehicle* APresentationFinalCharacter::GetPhysXVehicleFromController(APlayerController* inController)
{

	for (int32 i = 0; i < VehicleMapValues.Num(); i++)
	{
		APlayerController* PC = ControllerMapKeys[i];
		APhysXVehicle* V = VehicleMapValues[i];
	}

	VehicleControllerStruct.GetValueFromKey(inController);
	if (VehicleMapValues.Num() != 0)
	{
		return VehicleMapValues[0];
	}
	else
	{
		return nullptr;
	}

}



