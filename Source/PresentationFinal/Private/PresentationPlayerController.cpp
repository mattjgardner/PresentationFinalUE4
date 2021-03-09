// Fill out your copyright notice in the Description page of Project Settings.


#include "PresentationPlayerController.h"

APresentationPlayerController::APresentationPlayerController()
{
	bOnlyRelevantToOwner = false;
	bReplicates = true;
	bAlwaysRelevant = true;
}



APresentationFinalCharacter* APresentationPlayerController::GetPlayerCharacter()
{
	return ControlledCharacter;
}

void APresentationPlayerController::SetPlayerCharacter(APresentationFinalCharacter* inCharacter)
{
	ControlledCharacter = inCharacter;
}