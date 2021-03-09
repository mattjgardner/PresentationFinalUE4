// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "GameFramework/GameModeBase.h"
#include "PresentationFinalCharacter.h"
#include "PresentationFinalGameMode.generated.h"


USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FStartGameSessionState() {
		Status = false;
	}
};

USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY();

	FUpdateGameSessionState() {

	}
};

USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	long TerminationTime;

	FProcessTerminateState() {
		Status = false;
	}
};

USTRUCT()
struct FHealthCheckState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FHealthCheckState() {
		Status = false;
	}
};




UCLASS(minimalapi)
class APresentationFinalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APresentationFinalGameMode();


	//void SpawnAndPossessVehiclePawn(APresentationFinalCharacter* inCharacter, APlayerController* inPlayerController, TSubclassOf<APawn> inPawn);

	//UPROPERTY(Replicated)
	//bool bInvert;

	


	//UFUNCTION(NetMulticast, Reliable)
	//void UpdateInvertColour();
	//bool UpdateInvertColour_Validate();
	//void UpdateInvertColour_Implementation();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
		FStartGameSessionState StartGameSessionState;

	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
		FHealthCheckState HealthCheckState;
};



