#include "ChainQuestHandler.h"

#include "ChainQuestGraph.h"
#include "EndQuestInfo.h"
#include "QuestInfo.h"
#include "QuestTagsManager.h"
#include "QuestWorldSubsystem.h"

FChainQuestHandler::FChainQuestHandler(const UChainQuest* InChainQuest)
{
	ChainQuest = InChainQuest;

	//We get the start node
	UQuestRuntimeNode** StartNodePtr = ChainQuest->ChainQuestGraph->Nodes.FindByPredicate([](const UQuestRuntimeNode* Node)
	{
		return Node->QuestNodeType ==  EQuestNodeType::StartNode;
	});

	check(StartNodePtr);

	UQuestRuntimeNode* StartNode = *StartNodePtr;

	//We store the first quest node
	CurrentNode = StartNode->OutputPins[0]->Connection->Parent;
	QuestsInfos.Add(CurrentNode->QuestInfo);
}

bool FChainQuestHandler::CheckCurrentNodeConditions(UWorld* World)
{
	check(CurrentNode)

	UQuestInfo* Info = Cast<UQuestInfo>(CurrentNode->QuestInfo);

	TArray<TSubclassOf<UCondition>> Conditions;
	Info->OutPuts.GetKeys(Conditions);
	int Index = 0;
	
	for (const TSubclassOf<UCondition>& Condition : Conditions)
	{
		check(Condition);
		
		if (Condition.GetDefaultObject()->CheckCondition(World))
		{
			CurrentNode = CurrentNode->OutputPins[Index]->Connection->Parent;
			QuestsInfos.Add(CurrentNode->QuestInfo);
			
			if (CurrentNode->QuestNodeType == EQuestNodeType::QuestNode)
			{
				return CheckCurrentNodeConditions(World);
			}
			
			if(CurrentNode->QuestNodeType == EQuestNodeType::EndNode)
			{
				return true;
			}
		}
		
		Index++;
	}
	
	return false;
	
}

bool FChainQuestHandler::CheckCurrentEndDay(const int CurrentDay)
{
	if (CurrentNode->QuestNodeType == EQuestNodeType::EndNode)
	{
		return true;
	}

	if (!ChainQuest->bHasCalendarDates || Cast<UQuestInfo>(CurrentNode->QuestInfo)->DayToComplete == 0)
	{
		return false;
	}
	
	if (CurrentDay >= Cast<UQuestInfo>(CurrentNode->QuestInfo)->DayToComplete)
	{
		//By default the pin selected if the quest expires is the first one
		CurrentNode = CurrentNode->OutputPins[0]->Connection->Parent;
		QuestsInfos.Add(CurrentNode->QuestInfo);
		return CheckCurrentEndDay(CurrentDay);
	}

	return false;
	
}
