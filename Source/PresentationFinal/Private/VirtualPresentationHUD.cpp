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


	//bAddedToViewport = false;

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/ThirdPerson/Textures/FirstPersonCrosshair.FirstPersonCrosshair'"));
	CrosshairTex = CrosshairTexObj.Object;

	//static ConstructorHelpers::FObjectFinder<UUserWidget> IPWidgetObj(TEXT("WidgetBlueprint'/Game/UserInterface/UserInterfaceIP.UserInterfaceIP'"));
	//IPWidget = IPWidgetObj.Object;
	

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
	

	//IPWidget->AddToViewport();


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


void AVirtualPresentationHUD::AddWhiteboardEditorWidget()
{
	WhiteboardEditorWidget->AddToViewport();
	bAddedToViewport = true;

	//FString LevelName = "35.176.222.3:7777";

	//FGuid PlayerSessionGuid = FGuid::NewGuid();
	//FGuid PlayerGuid = FGuid::NewGuid();
	//
	//const FString& Options = "?PlayerSessionId=" + PlayerSessionGuid.ToString() + "?PlayerId" + PlayerGuid.ToString();
	//
	//UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);
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