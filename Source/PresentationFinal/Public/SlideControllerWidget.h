// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlideControllerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRESENTATIONFINAL_API USlideControllerWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	USlideControllerWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual TSharedRef<SWidget> RebuildWidget();

	UFUNCTION(BlueprintCallable)
		void SetPause();

	UFUNCTION(BlueprintCallable)
		int32 GetPause();

	UFUNCTION(BlueprintCallable)
		void StartSlideshow();

	UPROPERTY(EditAnywhere)
		UPanelWidget* RootWidget;

	UPROPERTY()
		int32 PauseIndex = 0;

	UPROPERTY()
		bool bIsStarted;

	UFUNCTION()
		bool GetBoolStarted();

};
