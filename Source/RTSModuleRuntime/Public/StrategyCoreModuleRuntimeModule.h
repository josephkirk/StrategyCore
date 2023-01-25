// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"

class UObject;

STRATEGYCOREMODULERUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogStrategyCore, Log, All);
STRATEGYCOREMODULERUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogStrategyCoreExperience, Log, All);

class FStrategyCoreModuleRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
