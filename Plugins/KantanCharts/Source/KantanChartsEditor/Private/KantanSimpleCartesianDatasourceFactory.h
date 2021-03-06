// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "KantanSimpleCartesianDatasourceFactory.generated.h"


/*
Factory class for simple cartesian data source asset.
*/
UCLASS()
class UKantanSimpleCartesianDatasourceFactory: public UFactory
{
	GENERATED_BODY()

public:
	UKantanSimpleCartesianDatasourceFactory(FObjectInitializer const&);

public:
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	// End of UFactory interface
};


