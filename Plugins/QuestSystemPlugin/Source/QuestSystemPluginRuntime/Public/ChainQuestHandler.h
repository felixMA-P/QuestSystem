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

	//returns false if the chain quest continues
	//returns true if the chain quest ends
	bool CheckCurrentNodeConditions();

	//returns false if the chain quest continues
	//returns true if the chain quest ends
	bool CheckCurrentEndDay(int CurrentDay);
	
public:
	
	FChainQuestHandler( const UChainQuest* InChainQuest);

	
};
