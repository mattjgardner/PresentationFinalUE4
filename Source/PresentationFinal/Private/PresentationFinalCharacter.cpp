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
	MarkerDrawSize = 0.01f;

	//AddToRoot();

	//UGameplayStatics::OpenLevel();
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	//apart from when moving left to right
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Sets the camera location to where the head of the third person mannequin should be 
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(19.f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Allows for overlap events to occur and binds a function to it
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APresentationFinalCharacter::BeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APresentationFinalCharacter::EndOverlap);

}

void APresentationFinalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Called every game tick functions explained above their implementation
	OnTickWhiteboardControlCheck();

	OnTickSlideControlCheck();

}


//Does checks to prevent crashes
//Then checks the playercontroller is local (not on a server)
//And checks the role is <ROLE_Authority (i.e. they're a client)
//Then calls a Server RPC to set certain whiteboard properties
//based on the widget values
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
								float WidgetDrawSize = WhiteboardEditorWidget->GetDrawSize();
								if (WidgetDrawSize == 0 || WidgetDrawSize == NULL)
								{
									MarkerDrawSize = 0.01;
								}
								else
								{
									MarkerDrawSize = WidgetDrawSize;
								}
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
							int32 ClearRender = WhiteboardEditorWidget->GetClearRender();
							if ((ClearRender % 2) != 0)
							{
								ServerClearRenderTarget(ClearRender);
							}
							
						}
					}
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

//Similar to above but controls the Slideshow using widget values
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
								if ((SlideControllerWidget->GetPause() % 2) != 0 )
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


//Function for defining what is replicated
//DOREPLIFETIME simply specifies to replicate the property
//DOREPLIFETIME_CONDITION_NOTIFY means it will be replicated even if it doesn't change
void APresentationFinalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APresentationFinalCharacter, bInvertColour, COND_None, REPNOTIFY_Always);
	

	DOREPLIFETIME(APresentationFinalCharacter, BoardOpacity);
	DOREPLIFETIME(APresentationFinalCharacter, ClearRenderTarget);
	DOREPLIFETIME(APresentationFinalCharacter, UVHitPosition);
	DOREPLIFETIME(APresentationFinalCharacter, MarkerDrawSize);
	DOREPLIFETIME(APresentationFinalCharacter, bStartSlideshow);
	DOREPLIFETIME(APresentationFinalCharacter, bPauseSlideshow);
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


//Begin Overlap dynamic -> draws widgets depending on what is being overlapped
//Has sufficient checks to check components are valid to prevent crashes
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
	else if (OtherActor->GetClass()->GetName() == "SlideActor" || OtherActor->GetClass()->GetName() == "SlideActor_BP")
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


//if either widget is valid, remove from the viewport so it's not constantly on screen
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


//When F is pressed, will allow for the cursor to be shown
//Pressed again it will make the mouse move the camera in the game
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

//Reduces the FOV when zooming in, increases it when zooming out up to a maximum of 90 degrees
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

//Setter functions for Drawing
//Gets checked in the board tick function before drawing to the board

void APresentationFinalCharacter::SetbDrawTrue()
{
	bDraw = true;
}

void APresentationFinalCharacter::SetbDrawFalse()
{
	bDraw = false;
}


//Draws to a whiteboard
//Uses the unit forward vector of the camera and does a line trace between the camera location
//and the 1500*unit forward vector (1500 is in cm) so you can't draw really far away
//then checks a boardactor has been hit and casts to ABoardActor.
//UGameplayStatics library finds where the hit occurs from the FHitResult

FVector2D APresentationFinalCharacter::Draw()
{
	FVector2D UVHit;
	if (bWhiteboardOverlap == true)
	{
		FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector ForwardVector = 1500 * FirstPersonCameraComponent->GetForwardVector();

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


//When bInvertColour is replicated, call the UpdateInvert
//function to set the invertcolour on the whiteboard
void APresentationFinalCharacter::OnRep_Invert()
{
	UpdateInvert();
}

void APresentationFinalCharacter::UpdateInvert()
{ 
	this->BoardActor->Client_OnRep_SetInvertColour(bInvertColour);
}

//Could do a check here (usually used in gaming to check for cheating)
//Haven't done it because I don't expect it to be exploited at any point
//If you return false, client gets kicked from server
bool APresentationFinalCharacter::ServerUpdateInvert_Validate(bool InbInvert)
{
	return true;
}

//Server RPC for client to pass parameter to server
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

void APresentationFinalCharacter::OnRep_ClearRenderTarget()
{
	UpdateClearRenderTarget();
}

void APresentationFinalCharacter::UpdateClearRenderTarget()
{
	this->BoardActor->ClearBoard();
}

bool APresentationFinalCharacter::ServerClearRenderTarget_Validate(int32 InbClearRender)
{
	return true;
}

void APresentationFinalCharacter::ServerClearRenderTarget_Implementation(int32 InClearRender)
{
	ClearRenderTarget = InClearRender;
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
	//UE_LOG(LogTemp, Log, TEXT("onrep"));
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

bool APresentationFinalCharacter::ServerSwitchPossessedPawn_Validate(APlayerController* inPlayerController, TSubclassOf<APawn> inPawn)
{
	return true;
}

//Unpossesses the player character and possesses the controlled vehicle
//if there is no controlled vehicle, calls the spawnpawn function to spawn one
void APresentationFinalCharacter::ServerSwitchPossessedPawn_Implementation(APlayerController* inPlayerController, TSubclassOf<class APawn> inPawn)
{
	//UE_LOG(LogTemp, Log, TEXT("Made it to server switch pawn"));
	inPlayerController->UnPossess();
	if (ControlledVehicle)
	{
		ControlledVehicle->SetPlayerCharacter(this);
		inPlayerController->Possess(ControlledVehicle);
	}
	else
	{
		ServerSpawnPawn(inPawn);
		ControlledVehicle->SetPlayerCharacter(this);
		inPlayerController->Possess(ControlledVehicle);
	}
}

//Originally was going to call a function when the vehicle struct replicates
//so all users have all vehicles on their gamestate
//this may be useful for filling graphs with multiple users data later
//however could be achieved by just sending the graph data and plotting it
//on the same axis
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

/* Will always spawn the vehicle but will adjust it if there's something in the way
* Finds the position of the SpawnLocationActor defined in the PhysXVehicle module
* using the GameplayStatics library and Casts it to AMultiplayerVehicleSpawnLocation
* as the GameplayStatics only returns AActor*
* Then spawns a vehicle at that lcoation
*/
void APresentationFinalCharacter::ServerSpawnPawn_Implementation(TSubclassOf<class APawn> inPawn)
{
	//UE_LOG(LogTemp, Log, TEXT("Made it to server spawn pawn"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMultiplayerVehicleSpawnLocation* SpawnLocationActor = Cast<AMultiplayerVehicleSpawnLocation>(UGameplayStatics::GetActorOfClass(GetWorld(), AMultiplayerVehicleSpawnLocation::StaticClass()));

	if(SpawnLocationActor)
	{
		APawn* const SpawnedPawn = GetWorld()->SpawnActor<APawn>(inPawn->GetDefaultObject()->GetClass(), SpawnLocationActor->LocationComponent->GetComponentLocation(), FRotator(0, 0, 0), SpawnParams);
		ControlledVehicle = Cast<APhysXVehicle>(SpawnedPawn);
	}
}


//Spawns a whiteboard only on the client that calls the function - a private whiteboard, if you will
//SpawnRotation is set from the ForwardVector of the character + 90 degrees in the Yaw axis
//It will always face the client
void APresentationFinalCharacter::ClientSpawnWhiteboard()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector SpawnLocation = FirstPersonCameraComponent->GetComponentLocation() + 400 * ForwardVector;

	FRotator SpawnRotation = ForwardVector.Rotation() + FRotator(0, 0, 90);

	GetWorld()->SpawnActor<ABoardActor>(ABoardActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
}



