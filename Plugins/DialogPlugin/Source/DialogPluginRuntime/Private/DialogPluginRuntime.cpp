// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogPluginRuntime.h"
#include "DialogTagsManager.h"

#define LOCTEXT_NAMESPACE "FDialogPluginRuntimeModule"

void FDialogPluginRuntimeModule::StartupModule()
{
	FDialogTagsManager::Get().InitGameplayTags();
}

void FDialogPluginRuntimeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDialogPluginRuntimeModule, DialogPluginRuntime)
