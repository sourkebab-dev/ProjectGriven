// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
// Copyright 2020 LunaticX, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRuntimeCopyMorphToolsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
