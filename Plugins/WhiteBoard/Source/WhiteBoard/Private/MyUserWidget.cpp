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

TSharedRef<SWidget> UMyUserWidget::RebuildWidget()
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



		if (TextboxSlot)
		{
			TextboxSlot->SetAutoSize(true);
		}

	}
	
	return Widget;
	
}

void UMyUserWidget::SetDrawSize(float InDrawSize)
{
	DrawSize = InDrawSize;
}


float UMyUserWidget::GetDrawSize()
{
	return DrawSize;
}

void UMyUserWidget::SetWhiteboardOpacity(float InOpacity)
{
	WhiteBoardOpacity = InOpacity;
}

float UMyUserWidget::GetWhiteboardOpacity()
{
	return WhiteBoardOpacity;
}

void UMyUserWidget::SetBoolInvert()
{
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
	return bInvertColour;
}
