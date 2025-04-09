#pragma once

#include "CoreMinimal.h"
#include "ChainQuest.h"
#include "QuestInfoBase.h"

class UQuestRuntimeNode;

class FChainQuestHandler
{

	friend class UQuestWorldSubsystem;

	const UChainQuest* ChainQuest;

	UQuestRuntimeNode* CurrentNode = nullptr;

	void CheckCurrentNodeConditions(UQuestWorldSubsystem* QuestSystem);
	
public:
	
	FChainQuestHandler( const UChainQuest* InChainQuest);

	
};
