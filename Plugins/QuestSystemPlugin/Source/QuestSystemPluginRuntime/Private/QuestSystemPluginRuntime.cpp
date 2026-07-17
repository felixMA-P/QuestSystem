// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "QuestSystemPluginRuntime.h"

#include "QuestTagsManager.h"

DEFINE_LOG_CATEGORY(LogQuestSystem);

#define LOCTEXT_NAMESPACE "FQuestSystemPluginRuntimeModule"

void FQuestSystemPluginRuntimeModule::StartupModule()
{
	FQuestTagsManager::Get().InitGameplayTags();
}

void FQuestSystemPluginRuntimeModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FQuestSystemPluginRuntimeModule, QuestSystemPluginRuntime)