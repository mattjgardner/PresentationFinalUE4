// Copyright Epic Games, Inc. All Rights Reserved.

#include "VirtualPresentationHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Blueprint/UserWidget.h"
#include <WhiteBoard/Public/MyUserWidget.h>
#include "SlideControllerWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Guid.h"

AVirtualPresentationHUD::AVirtualPresentationHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/ThirdPerson/Textures/FirstPersonCrosshair.FirstPersonCrosshair'"));
	CrosshairTex = CrosshairTexObj.Object;

}


void AVirtualPresentationHUD::DrawHUD()
{
	Super::DrawHUD();

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - 6),
		(Center.Y - 6));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
	
	

	//If the custom widgets are nullptr, create them and call the RebuildWidget function to fill the widget.

	if (WhiteboardEditorWidget == nullptr)
	{
		WhiteboardEditorWidget = CreateWidget<UMyUserWidget>(GetOwner()->GetWorld(), UMyUserWidget::StaticClass());
		WhiteboardEditorWidget->RebuildWidget();
	}

	if (SlideControllerWidget == nullptr)
	{
		SlideControllerWidget = CreateWidget<USlideControllerWidget>(GetOwner()->GetWorld(), USlideControllerWidget::StaticClass());
		SlideControllerWidget->RebuildWidget();
	}
}


//Custom functions to add and remove widgets from the HUD

void AVirtualPresentationHUD::AddWhiteboardEditorWidget()
{
	WhiteboardEditorWidget->AddToViewport();
	bAddedToViewport = true;
}


void AVirtualPresentationHUD::RemoveWhiteboardEditorWidget()
{
	WhiteboardEditorWidget->RemoveFromViewport();
	bAddedToViewport = false;
}



UMyUserWidget* AVirtualPresentationHUD::GetWhiteboardEditorWidget()
{
	return WhiteboardEditorWidget;
}


void AVirtualPresentationHUD::RemoveSlideControllerWidget()
{
	SlideControllerWidget->RemoveFromViewport();
	bAddedSlideWidgetToViewport = false;
	
}

void AVirtualPresentationHUD::AddSlideControllerWidget()
{
	SlideControllerWidget->AddToViewport();
	bAddedSlideWidgetToViewport = true;

}

USlideControllerWidget* AVirtualPresentationHUD::GetSlideControllerWidget()
{
	return SlideControllerWidget;
}