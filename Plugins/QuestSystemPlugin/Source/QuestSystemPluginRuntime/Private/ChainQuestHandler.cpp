// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "ChainQuestHandler.h"

#include "ChainQuestGraph.h"
#include "EndQuestInfo.h"
#include "QuestEvent.h"
#include "QuestInfo.h"
#include "QuestSystemPluginRuntime.h"
#include "QuestTagsManager.h"
#include "QuestWorldSubsystem.h"
#include "SystemFunctionHelper.h"

FChainQuestHandler::FChainQuestHandler(const UChainQuest* InChainQuest)
{
	ChainQuest = InChainQuest;

	if (!ChainQuest->ChainQuestGraph)
	{
		UE_LOG(LogQuestSystem, Error, TEXT("Quest System: FChainQuestHandler - ChainQuest '%s' has no ChainQuestGraph"), *ChainQuest->Title.ToString());
		return;
	}

	//We get the start node
	UQuestRuntimeNode** StartNodePtr = ChainQuest->ChainQuestGraph->Nodes.FindByPredicate([](const UQuestRuntimeNode* Node)
	{
		return Node && Node->QuestNodeType ==  EQuestNodeType::StartNode;
	});

	if (!StartNodePtr)
	{
		UE_LOG(LogQuestSystem, Error, TEXT("Quest System: FChainQuestHandler - ChainQuest '%s' has no StartNode"), *ChainQuest->Title.ToString());
		return;
	}

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

	int Index = 0;

	for (const FQuestOutput& Output : Info->OutPuts)
	{
		bool bConditionMet;
		if (ChainQuest->bUseSimpleConditions)
		{
			if (!Output.ConditionTag.IsValid()) { Index++; continue; }

			const UQuestWorldSubsystem* QuestSystem = USystemFunctionHelper::GetQuestSystem(World);
			bConditionMet = QuestSystem && QuestSystem->QuestGameplayTagsContainer.HasTagExact(Output.ConditionTag);
		}
		else
		{
			if (!Output.Condition) { Index++; continue; }

			bConditionMet = Output.Condition.GetDefaultObject()->CheckCondition(World);
		}

		if (bConditionMet)
		{
			if (!CurrentNode->OutputPins.IsValidIndex(Index) || !CurrentNode->OutputPins[Index]->Connection || !CurrentNode->OutputPins[Index]->Connection->Parent)
			{
				return false;
			}

			if (Output.Event)
			{
				Output.Event.GetDefaultObject()->Execute(World);
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

bool FChainQuestHandler::CheckCurrentEndDay(const int CurrentDay, int Depth)
{
	if (Depth > 64)
	{
		UE_LOG(LogQuestSystem, Warning, TEXT("FChainQuestHandler: max recursion depth reached — possible cycle in quest graph"));
		return false;
	}

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
		return CheckCurrentEndDay(CurrentDay, Depth + 1);
	}

	return false;
}
