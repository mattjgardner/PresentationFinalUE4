// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"


class FKantanTimeSeriesPlotCustomization: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	// Keep a pointer for refreshing layout
	IDetailLayoutBuilder* DetailBuilderPtr = nullptr;
};


