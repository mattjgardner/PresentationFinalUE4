// Copyright Epic Games, Inc. All Rights Reserved.

#include "PresentationFinalGameMode.h"
#include "PresentationFinalCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "VirtualPresentationHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PhysXVehicle.h"
#include "MultiplayerVehicleSpawnLocation.h"
#include <WhiteBoard/Public/BoardActor.h>
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

APresentationFinalGameMode::APresentationFinalGameMode()
{
	//bReplicates = true;
	//bAlwaysRelevant = true;

	//bInvert = false;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//Sets the HUD to the HUD I created
	HUDClass = AVirtualPresentationHUD::StaticClass();



}


void APresentationFinalGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}


void APresentationFinalGameMode::BeginPlay() {
	Super::BeginPlay();

//Code doesn't get compiled even with the editor but will be compiled for the server build
//Based on code created by Flopperam and the tutorial series created on YouTube
//auto used extensively to deduce the type of the return
//else return types will be similar to Aws::GameLift::Server::InitSDKOutcome
//which isn't particularly readable either

#if WITH_GAMELIFT





auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

//Checks the SDK is initialised
if (InitSDKOutcome.IsSuccess()) {
	
	//GameLift invokes this function when notifying server process that a game session is being started
	auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
	{
		//Casts Params from void* to the struct defined in the header file
		FStartGameSessionState* State = (FStartGameSessionState*)Params;

		//Onstart game session is called server process calls activategamesession
		//When successful, it notifies the game session that it is ready to recieve new player
		//connections
		//Status is stored in the struct
		State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
	};

	//OnUpdateGameSession invoked by GameLift when match backfill request is fulfilled 
	//i.e. if 2 players join then 2 players are trying to join it will be invoked
	//The GameSessionObjective will contain data of all 4 players
	auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params)
	{
		FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;
	};

	//Invoked by GameLift for 3 reasons:
	//-Server process is unhealthy for 3 consecutive minutes from OnHealthCheck callback function
	//-Server process is on instance that experiences a "Spot interruption": Only applies to spot fleets
	//not On-Demand fleets
	//-Fleet instance capacity has been scaled down - can be automatically or manually invoked from the
	//AWS Dashboard
	auto OnProcessTerminate = [](void* Params)
	{
		FProcessTerminateState* State = (FProcessTerminateState*)Params;

		//Gets the time it was terminated
		auto GetTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();
		
		//Check it is sucessful
		if (GetTerminationTimeOutcome.IsSuccess()) {
			State->TerminationTime = GetTerminationTimeOutcome.GetResult();
		}

		//Lets server know that process is terminating
		auto ProcessEndingOutcome = Aws::GameLift::Server::ProcessEnding();

		if (ProcessEndingOutcome.IsSuccess()) {
			State->Status = true;
			FGenericPlatformMisc::RequestExit(false);
		}
	};

	//Invoked by GameLift service every minute to check the health of the server
	auto OnHealthCheck = [](void* Params)
	{
		FHealthCheckState* State = (FHealthCheckState*)Params;
		State->Status = true;

		return State->Status;
	};


	//Tell GameLift what port this server process will use on this instance
	//Uses command line
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	int Port = FURL::UrlConfig.DefaultPort;


	
	// GameLiftTutorialServer.exe token -port=7777
	//CommandLineTokens are any argument not preceeded by a dash
	//in the case above, GameLiftTutorial.exe and token are CommandLineTokens
	
	//CommandLineSwitches are any argumend preceeded by a dash e.g. -port=7777
	// in the case above
	//The FCommandLine::Parse function requires a TArray of tokens and switches
	//However we only care about the switches with a key and a value
	//So we will iterate through them to find the port of the server
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	for (int32 i = 0; i < CommandLineSwitches.Num(); i++)
	{
		FString Key;
		FString Value;

		if (CommandLineSwitches[i].Split("=", &Key, &Value))
		{
			if (Key.Equals("port"))
			{
				//Atoi converts string to an integer
				Port = FCString::Atoi(*Value);
			}
		}
	}



	//for (FString Str : CommandLineSwitches) {
	//	
	//	FString Key;
	//	FString Value;

	//	if (Str.Split("=", &Key, &Value)) {
	//		if (Key.Equals("port")) {
	//			//Atoi converts string to an integer
	//			Port = FCString::Atoi(*Value);
	//		}
	//	}
	//}


	const char* LogFile = "ServerLogFile.txt";

	//Gets address of logfile
	//LogFileAddress
	const char** LogFiles = &LogFile;

	auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

	//Creates process parameters using the callback functions
	//created above
	auto Params = new Aws::GameLift::Server::ProcessParameters(
		OnStartGameSession,
		&StartGameSessionState,
		OnUpdateGameSession,
		&UpdateGameSessionState,
		OnProcessTerminate,
		&ProcessTerminateState,
		OnHealthCheck,
		&HealthCheckState,
		Port,
		*LogParams
	);

	//could be set to a variable if you want to handle cases of 
	//unsucessful set up. However, since I won't be it will be left
	//as just a function call
	Aws::GameLift::Server::ProcessReady(*Params);
}
#endif
}
