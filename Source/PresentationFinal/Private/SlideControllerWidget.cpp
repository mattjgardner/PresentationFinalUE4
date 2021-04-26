// Fill out your copyright notice in the Description page of Project Settings.


#include "SlideControllerWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

USlideControllerWidget::USlideControllerWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {
	
}

void USlideControllerWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


//Rebuild Widget described in the Whiteboard.cpp file
TSharedRef<SWidget> USlideControllerWidget::RebuildWidget()
{

	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (!RootWidget)
	{
		RootWidget = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootWidget"));

		UCanvasPanelSlot* RootWidgetSlot = Cast<UCanvasPanelSlot>(RootWidget->Slot);

		if (RootWidgetSlot)
		{
			RootWidgetSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			RootWidgetSlot->SetOffsets(FMargin(0.f, 0.f));


		}
		WidgetTree->RootWidget = RootWidget;
	}

	if (RootWidget && WidgetTree)
	{
		UTextBlock* PauseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("PauseText")));
		RootWidget->AddChild(PauseText);
		PauseText->SetText(FText::FromString("Pause Presentation"));

		UCanvasPanelSlot* PauseTextSlot = Cast<UCanvasPanelSlot>(PauseText->Slot);
		PauseTextSlot->SetAnchors(FAnchors(0.7f, 0.05f));
		PauseTextSlot->SetOffsets(FMargin(0.f, 0.f));


		UButton* PauseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("PauseButton"));
		RootWidget->AddChild(PauseButton);
		PauseButton->OnClicked.AddDynamic(this, &USlideControllerWidget::SetPause);

		UCanvasPanelSlot* PauseButtonSlot = Cast<UCanvasPanelSlot>(PauseButton->Slot);
		PauseButtonSlot->SetAutoSize(false);
		PauseButtonSlot->SetAnchors(FAnchors(0.7f, 0.1f)); //should be enough space
		PauseButtonSlot->SetOffsets(FMargin(0.f, 0.f));
		PauseButtonSlot->SetSize(FVector2D(100.f, 100.f));

		UTextBlock* HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("HintText")));
		RootWidget->AddChild(HintText);
		HintText->SetText(FText::FromString("Start Presentation"));

		UCanvasPanelSlot* HintTextSlot = Cast<UCanvasPanelSlot>(HintText->Slot);
		HintTextSlot->SetAnchors(FAnchors(0.7f, 0.35f));
		HintTextSlot->SetOffsets(FMargin(0.f, 0.f));

		UButton* StartButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("StartButton"));
		RootWidget->AddChild(StartButton);
		StartButton->OnClicked.AddDynamic(this, &USlideControllerWidget::StartSlideshow);

		UCanvasPanelSlot* StartButtonSlot = Cast<UCanvasPanelSlot>(StartButton->Slot);
		StartButtonSlot->SetAutoSize(false);
		StartButtonSlot->SetAnchors(FAnchors(0.7f, 0.4f)); //should be enough space
		StartButtonSlot->SetOffsets(FMargin(0.f, 0.f));
		StartButtonSlot->SetSize(FVector2D(100.f, 100.f));

	}

	return Widget;
}

//Adds 1 to the PauseIndex
//Pausing only works when virtual host is not used
void USlideControllerWidget::SetPause()
{
	PauseIndex++;
}

//Returns the pause index.
//This is called in PresentationFinalCharacter
//if PauseIndex % 2 != 0 the presentation is paused
//if PauseIndex % 2 == 0 the presentation is unpaused
int32 USlideControllerWidget::GetPause()
{
	return PauseIndex;
}

void USlideControllerWidget::StartSlideshow()
{
	if (bIsStarted == false)
	{
		bIsStarted = true;
	}
	else
	{
		bIsStarted = false;
	}
}

bool USlideControllerWidget::GetBoolStarted()
{
	return bIsStarted;
}