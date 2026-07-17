// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FDialogPluginRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
