#include "QuestSystemPluginRuntime.h"

#include "QuestTagsManager.h"

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