// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VirtualPresentationHUD.generated.h"


UCLASS()
class AVirtualPresentationHUD : public AHUD
{
	GENERATED_BODY()



public:
	AVirtualPresentationHUD();

	//Whiteboard editor widget added to viewport
	UPROPERTY()
		bool bAddedToViewport;

	//Slide editor widget added to viewport
	UPROPERTY()
		bool bAddedSlideWidgetToViewport;

	UPROPERTY(EditAnywhere)
		class UMyUserWidget* WhiteboardEditorWidget;

	UPROPERTY(EditAnywhere)
		class USlideControllerWidget* SlideControllerWidget;
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	void AddWhiteboardEditorWidget();

	void RemoveWhiteboardEditorWidget();
	
	void AddSlideControllerWidget();

	void RemoveSlideControllerWidget();


	UFUNCTION()
		UMyUserWidget* GetWhiteboardEditorWidget();

	UFUNCTION()
		USlideControllerWidget* GetSlideControllerWidget();

private:
	class UTexture2D* CrosshairTex;



};
