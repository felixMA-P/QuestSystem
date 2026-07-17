// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChainQuest.h"
#include "QuestInfoBase.h"

class UQuestRuntimeNode;
class UQuestWorldSubsystem;

class FChainQuestHandler
{
	friend class UQuestWorldSubsystem;

	const UChainQuest* ChainQuest;

	UQuestRuntimeNode* CurrentNode = nullptr;
	
	TArray<const UQuestInfoBase*> QuestsInfos;

	//returns false if the chain quest continues
	//returns true if the chain quest ends
	bool CheckCurrentNodeConditions(UWorld* World, int Depth = 0);

	//returns false if the chain quest continues
	//returns true if the chain quest ends
	bool CheckCurrentEndDay(int CurrentDay, int Depth = 0);
	
public:

	const UChainQuest* GetChainQuest() const { return ChainQuest; }
	
	FChainQuestHandler( const UChainQuest* InChainQuest);

	
};
