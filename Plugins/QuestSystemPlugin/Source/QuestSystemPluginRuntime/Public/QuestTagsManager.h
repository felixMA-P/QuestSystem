#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

class FQuestTagsManager : public FNoncopyable
{
public:

	static FQuestTagsManager& Get()
	{
		static FQuestTagsManager* Singleton;
		
		if (!Singleton)
		{
			Singleton = new FQuestTagsManager();
		}
		
		return *Singleton;
	}

	void InitGameplayTags()
	{
		QuestStatusEnabled = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.QuestSystem.Quest.Status.Enabled"));
		QuestStatusDisabled = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.QuestSystem.Quest.Status.Disabled"));
		QuestStatusOnGoing = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.QuestSystem.Quest.Status.OnGoing"));
		QuestStatusCompleted = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.QuestSystem.Quest.Status.Completed"));
	}
	
	
	FGameplayTag QuestStatusEnabled;
	FGameplayTag QuestStatusDisabled;
	FGameplayTag QuestStatusOnGoing;
	FGameplayTag QuestStatusCompleted;
	
};
