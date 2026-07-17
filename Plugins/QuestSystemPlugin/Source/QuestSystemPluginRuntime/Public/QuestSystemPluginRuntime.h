// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

QUESTSYSTEMPLUGINRUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogQuestSystem, Log, All);

class FQuestSystemPluginRuntimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
