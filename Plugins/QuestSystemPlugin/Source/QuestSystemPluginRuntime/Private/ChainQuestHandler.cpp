#include "ChainQuestHandler.h"

#include "ChainQuestGraph.h"
#include "EndQuestInfo.h"
#include "QuestInfo.h"
#include "QuestSystemPluginRuntime.h"
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

	if (StartNode->OutputPins.IsEmpty() || !StartNode->OutputPins[0]->Connection || !StartNode->OutputPins[0]->Connection->Parent)
	{
		return;
	}

	CurrentNode = StartNode->OutputPins[0]->Connection->Parent;
	QuestsInfos.Add(CurrentNode->QuestInfo);
}

bool FChainQuestHandler::CheckCurrentNodeConditions(UWorld* World, int Depth)
{
	if (Depth > 64)
	{
		UE_LOG(LogQuestSystem, Warning, TEXT("FChainQuestHandler: max recursion depth reached — possible cycle in quest graph"));
		return false;
	}

	if (!CurrentNode) { return false; }

	UQuestInfo* Info = Cast<UQuestInfo>(CurrentNode->QuestInfo);
	if (!Info) { return false; }

	TArray<TSubclassOf<UCondition>> Conditions;
	Info->OutPuts.GetKeys(Conditions);
	int Index = 0;

	for (const TSubclassOf<UCondition>& Condition : Conditions)
	{
		if (!Condition) { Index++; continue; }

		if (Condition.GetDefaultObject()->CheckCondition(World))
		{
			if (!CurrentNode->OutputPins.IsValidIndex(Index) || !CurrentNode->OutputPins[Index]->Connection || !CurrentNode->OutputPins[Index]->Connection->Parent)
			{
				return false;
			}

			CurrentNode = CurrentNode->OutputPins[Index]->Connection->Parent;
			QuestsInfos.Add(CurrentNode->QuestInfo);

			if (CurrentNode->QuestNodeType == EQuestNodeType::QuestNode)
			{
				return CheckCurrentNodeConditions(World, Depth + 1);
			}

			if (CurrentNode->QuestNodeType == EQuestNodeType::EndNode)
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
	if (!CurrentNode) { return false; }

	if (CurrentNode->QuestNodeType == EQuestNodeType::EndNode)
	{
		return true;
	}

	UQuestInfo* Info = Cast<UQuestInfo>(CurrentNode->QuestInfo);
	if (!Info) { return false; }

	if (!ChainQuest->bHasCalendarDates || Info->DayToComplete == 0)
	{
		return false;
	}

	if (CurrentDay >= Info->DayToComplete)
	{
		if (CurrentNode->OutputPins.IsEmpty() || !CurrentNode->OutputPins[0]->Connection || !CurrentNode->OutputPins[0]->Connection->Parent)
		{
			return false;
		}

		CurrentNode = CurrentNode->OutputPins[0]->Connection->Parent;
		QuestsInfos.Add(CurrentNode->QuestInfo);
		return CheckCurrentEndDay(CurrentDay);
	}

	return false;
}
