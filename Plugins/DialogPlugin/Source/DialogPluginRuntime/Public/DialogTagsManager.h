// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

class FDialogTagsManager : public FNoncopyable
{
public:
	static FDialogTagsManager& Get()
	{
		static FDialogTagsManager* Singleton;
		if (!Singleton)
			Singleton = new FDialogTagsManager();
		return *Singleton;
	}

	void InitGameplayTags()
	{
		DialogStatusActive    = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.DialogSystem.Dialog.Status.Active"));
		DialogStatusCompleted = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.DialogSystem.Dialog.Status.Completed"));
		DialogStatusLocked    = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Gameplay.DialogSystem.Dialog.Status.Locked"));
	}

	FGameplayTag DialogStatusActive;
	FGameplayTag DialogStatusCompleted;
	FGameplayTag DialogStatusLocked;
};
