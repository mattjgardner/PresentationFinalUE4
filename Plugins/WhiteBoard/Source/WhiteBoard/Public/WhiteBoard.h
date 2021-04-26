// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

//Loads the module on startup and if in the editor, calls the create whiteboard function

class FWhiteBoardModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

//Sets up the materials used in the whiteboard and whiteboard pen if called from the editor.
//This code is not compiled in the packaged product

#if WITH_EDITOR
	void CreateWhiteboard();
#endif
};
