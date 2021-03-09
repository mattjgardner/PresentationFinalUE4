// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCategoryChart.h"
#include "Charts/SKantanBarChart.h"
#include "KantanBarChartBase.generated.h"


UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanBarChartBase : public UKantanCategoryChart
{
	GENERATED_BODY()

public:
	// Style
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", Meta = (DisplayName = "Style"))
	FKantanBarChartStyle WidgetStyle;

	// Chart orientation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	EKantanBarChartOrientation Orientation;

	// Fixed maximum value for the bar chart (if 0, bars are normalized against the highest value at the time)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float MaxBarValue;

	//Minimum Value, must be set for negative graphs or negative values can't be read
	//Ideally if 0, bars will be normalised to the lowest value at the time but time is short
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float MinBarValue;

	// Positioning of bar labels
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	EKantanBarLabelPosition LabelPosition;

	// Ratio of the thickness of a bar to the thickness of gaps between bars
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float BarToGapRatio;

	// Whether to draw lines at the zero and max value positions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	EKantanBarValueExtents ValueExtentsDisplay;

	/*
	Value axis configuration.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Meta = (DisplayName = "Value Axis", DisableFloating))
	FCartesianAxisConfig ValueAxisCfg;

public:
	UKantanBarChartBase(FObjectInitializer const&);

public:
	/*
	Set chart orientation.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetOrientation(EKantanBarChartOrientation InOrientation);

	/*
	Set maximum bar value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetMaxBarValue(float InMaxValue);

	//My addition to this module
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetMinBarValue(float InMinValue);

	/*
	Set positioning of bar labels.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetLabelPosition(EKantanBarLabelPosition InPosition);

	/*
	Set ratio between bar width and gap between bars.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetBarToGapRatio(float InRatio);

	/*
	Set which chart extent lines are drawn.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetExtentsDisplay(EKantanBarValueExtents InExtents);

	/*
	Set value axis configuration.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetValueAxisConfig(FCartesianAxisConfig const& InCfg);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

protected:
	// UWidget interface
	virtual TSharedRef< SWidget > RebuildWidget() override;
	// End of UWidget interface

	inline TSharedPtr< SKantanBarChart > GetBarChart()
	{
		return StaticCastSharedPtr< SKantanBarChart >(MyChart);
	}
};


