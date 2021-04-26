// Fill out your copyright notice in the Description page of Project Settings.


#include "PresentationPlayerController.h"

APresentationPlayerController::APresentationPlayerController()
{
	bOnlyRelevantToOwner = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	
	//AddToRoot();
}


//Getter and setters to save the controlled character when switching to the vehicle
//This is called in the Vehicle blueprint graph when V is pressed to repossess
//the character and control the character compared to the vehicle again
APresentationFinalCharacter* APresentationPlayerController::GetPlayerCharacter()
{
	return ControlledCharacter;
}

void APresentationPlayerController::SetPlayerCharacter(APresentationFinalCharacter* inCharacter)
{
	ControlledCharacter = inCharacter;
}