// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PresentationFinalCharacter.h"
#include "GameFramework/PlayerController.h"
#include "PresentationPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class PRESENTATIONFINAL_API APresentationPlayerController : public APlayerController
{
	GENERATED_BODY()

		APresentationPlayerController();
	
public:

	UPROPERTY(BlueprintReadWrite)
	class APresentationFinalCharacter* ControlledCharacter;

	UFUNCTION(BlueprintCallable)
	APresentationFinalCharacter* GetPlayerCharacter(); 

	UFUNCTION(BlueprintCallable)
		void SetPlayerCharacter(APresentationFinalCharacter* inCharacter);


};
