// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WHITEBOARD_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual TSharedRef<SWidget> RebuildWidget();

	//Alot of these functions are BlueprintCallable for the opportunity to be called in blueprint graphs
	//however all have been called with C++

	UFUNCTION(BlueprintCallable)
	void SetDrawSize(float InDrawSize);

	UFUNCTION(BlueprintCallable)
	float GetDrawSize();

	UFUNCTION(BlueprintCallable)
	void SetWhiteboardOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable)
	float GetWhiteboardOpacity();

	UFUNCTION(BlueprintCallable)
	void SetBoolInvert();

	UFUNCTION(BlueprintCallable)
	bool GetBoolInvert();

	UFUNCTION(BlueprintCallable)
	void SetClearRenderTarget();

	UFUNCTION(BlueprintCallable)
	int32 GetClearRender();

	UPROPERTY(EditAnywhere)
	float DrawSize;

	UPROPERTY(EditAnywhere)
	float WhiteBoardOpacity;

	UPROPERTY()
	bool bInvertColour;

	UPROPERTY(EditAnywhere)
	int32 ClearRender = 0;

	UPROPERTY(EditAnywhere)
	UPanelWidget* RootWidget;
		
};
