// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
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


UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer){
}

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

//Calls the underlying slate to rebuild the widget
TSharedRef<SWidget> UMyUserWidget::RebuildWidget()
{
	//returns the widget
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	//Creates a rootwidget element 
	RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (!RootWidget)
	{
		//If the rootwidget isn't valid, makes one from the static class
		RootWidget = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootWidget"));

		//Creates a slot for the root widget on the screen
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
		/*For each component creates the component using the contruct widget and adds it to the root widget
		Then sets parameters to the component
		Then creates a slot for it to define where it is in the widget/on the screen
		
		.AddDynamic binds a function to the slider/button. With buttons, no parameters can be passed. With sliders
		the slider value is passed in to the function*/

		UTextBlock* EditorText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("EditorText")));
		RootWidget->AddChild(EditorText);
		EditorText->SetText(FText::FromString("Whiteboard Parameter Editor"));
		UCanvasPanelSlot* TextboxSlot = Cast<UCanvasPanelSlot>(EditorText->Slot);

		UTextBlock* HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("HintText")));
		RootWidget->AddChild(HintText);
		HintText->SetText(FText::FromString("Press F to enable cursor input"));

		UCanvasPanelSlot* HintTextSlot = Cast<UCanvasPanelSlot>(HintText->Slot);
		HintTextSlot->SetAnchors(FAnchors(0.3f, 0.05f));
		HintTextSlot->SetOffsets(FMargin(0.f, 0.f));

		UTextBlock* DrawSizeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("DrawSizeText")));
		RootWidget->AddChild(DrawSizeText);
		DrawSizeText->SetText(FText::FromString("Draw Size"));

		UCanvasPanelSlot* DrawSizeTextSlot = Cast<UCanvasPanelSlot>(DrawSizeText->Slot);
		DrawSizeTextSlot->SetAnchors(FAnchors(0.f, 0.1f)); 
		DrawSizeTextSlot->SetOffsets(FMargin(0.f, 0.f));

		USlider* DrawSizeSlider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("DrawSizeSlider"));
		RootWidget->AddChild(DrawSizeSlider);
		DrawSizeSlider->SetIndentHandle(true);
		DrawSizeSlider->SetStepSize(0.01);
		DrawSizeSlider->SetMinValue(0.01);
		DrawSizeSlider->SetMaxValue(0.2);
		DrawSizeSlider->SetValue(0.01);
		DrawSizeSlider->WidgetStyle.BarThickness = 6.0f;
		DrawSizeSlider->OnValueChanged.AddDynamic(this, &UMyUserWidget::SetDrawSize);
		

		UCanvasPanelSlot* DrawSizeSliderSlot = Cast<UCanvasPanelSlot>(DrawSizeSlider->Slot);
		DrawSizeSliderSlot->SetAutoSize(false);
		DrawSizeSliderSlot->SetAnchors(FAnchors(0.f, 0.15f)); 
		DrawSizeSliderSlot->SetOffsets(FMargin(0.f, 0.f));
		DrawSizeSliderSlot->SetSize(FVector2D(200.f, 10.f));

		

		UTextBlock* BoardOpacityText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("BoardOpacityText")));
		RootWidget->AddChild(BoardOpacityText);
		BoardOpacityText->SetText(FText::FromString("Board Opacity"));

		UCanvasPanelSlot* BoardOpacityTextSlot = Cast<UCanvasPanelSlot>(BoardOpacityText->Slot);
		BoardOpacityTextSlot->SetAnchors(FAnchors(0.f, 0.3f)); //should be enough space
		BoardOpacityTextSlot->SetOffsets(FMargin(0.f, 0.f));

		USlider* OpacitySlider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("OpacitySlider"));
		RootWidget->AddChild(OpacitySlider);
		OpacitySlider->SetIndentHandle(true);
		OpacitySlider->SetStepSize(0.05);
		OpacitySlider->SetMinValue(0.2);
		OpacitySlider->SetMaxValue(1);
		OpacitySlider->SetValue(0.7);
		OpacitySlider->Value = 0.7;
		OpacitySlider->WidgetStyle.BarThickness = 6.0f;
		OpacitySlider->OnValueChanged.AddDynamic(this, &UMyUserWidget::SetWhiteboardOpacity);

		UCanvasPanelSlot* OpacitySliderSlot = Cast<UCanvasPanelSlot>(OpacitySlider->Slot);
		OpacitySliderSlot->SetAutoSize(false);
		OpacitySliderSlot->SetAnchors(FAnchors(0.f, 0.35f)); //should be enough space
		OpacitySliderSlot->SetOffsets(FMargin(0.f, 0.f));
		OpacitySliderSlot->SetSize(FVector2D(200.f, 10.f));

		UButton* InvertColourButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("InvertColourButton"));
		RootWidget->AddChild(InvertColourButton);
		InvertColourButton->OnClicked.AddDynamic(this, &UMyUserWidget::SetBoolInvert);

		UCanvasPanelSlot* InvertColourButtonSlot = Cast<UCanvasPanelSlot>(InvertColourButton->Slot);
		InvertColourButtonSlot->SetAutoSize(false);
		InvertColourButtonSlot->SetAnchors(FAnchors(0.f, 0.4f)); //should be enough space
		InvertColourButtonSlot->SetOffsets(FMargin(0.f, 0.f));
		InvertColourButtonSlot->SetSize(FVector2D(100.f, 100.f));

		UButton* ClearRenderTargetButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ClearRenderTargetButton"));
		RootWidget->AddChild(ClearRenderTargetButton);
		ClearRenderTargetButton->OnClicked.AddDynamic(this, &UMyUserWidget::SetClearRenderTarget);


		UTextBlock* ClearRenderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(TEXT("ClearRenderTargetText")));
		RootWidget->AddChild(ClearRenderText);
		ClearRenderText->SetText(FText::FromString("Clear Render Target"));

		UCanvasPanelSlot* ClearRenderTextSlot = Cast<UCanvasPanelSlot>(ClearRenderText->Slot);
		ClearRenderTextSlot->SetAnchors(FAnchors(0.f, 0.55f)); //should be enough space
		ClearRenderTextSlot->SetOffsets(FMargin(0.f, 0.f));

		UCanvasPanelSlot* ClearRenderTargetButtonSlot = Cast<UCanvasPanelSlot>(ClearRenderTargetButton->Slot);
		ClearRenderTargetButtonSlot->SetAutoSize(false);
		ClearRenderTargetButtonSlot->SetAnchors(FAnchors(0.f, 0.6f)); //should be enough space
		ClearRenderTargetButtonSlot->SetOffsets(FMargin(0.f, 0.f));
		ClearRenderTargetButtonSlot->SetSize(FVector2D(100.f, 100.f));



		if (TextboxSlot)
		{
			TextboxSlot->SetAutoSize(true);
		}

	}
	
	return Widget;
	
}

void UMyUserWidget::SetDrawSize(float InDrawSize)
{
	//Takes slider value to set the draw size
	DrawSize = InDrawSize;
}


float UMyUserWidget::GetDrawSize()
{
	//returns draw size (called in PresentationFinalCharacter)
	if (DrawSize != 0 || DrawSize != NULL)
	{
		return DrawSize;
	}
	else
	{
		return 0.01f;
	}
}

void UMyUserWidget::SetWhiteboardOpacity(float InOpacity)
{
	//Sets opacity based on slider value
	WhiteBoardOpacity = InOpacity;
}

float UMyUserWidget::GetWhiteboardOpacity()
{
	//Called in PresentationFinalCharacter (PFC) to be replicated to other users
	return WhiteBoardOpacity;
}

void UMyUserWidget::SetBoolInvert()
{
	//Sets the colour of the board from a whiteboard to blackboard and vice versa
	if (bInvertColour == false)
	{
		bInvertColour = true;
	}
	else
	{
		bInvertColour = false;
	}
}

bool UMyUserWidget::GetBoolInvert()
{
	//Called in PFC 
	return bInvertColour;
}

void UMyUserWidget::SetClearRenderTarget()
{
	//Actually better than using a bool as the bool can cause bugs due to not being replicated
	//since replication only occurs when there's a change
	//However, user has to double click to clear the render target due to PFC function
	// (ClearRender % 2) == 0 <<< happens every second number
	ClearRender++;
}

int32 UMyUserWidget::GetClearRender()
{
	//Called in PFC	
	return ClearRender;
}