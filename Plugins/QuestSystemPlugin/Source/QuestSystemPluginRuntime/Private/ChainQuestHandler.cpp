#include "ChainQuestHandler.h"

#include "ChainQuestGraph.h"
#include "QuestInfo.h"
#include "QuestTagsManager.h"

FChainQuestHandler::FChainQuestHandler(const UChainQuest* InChainQuest)
{
	ChainQuest = InChainQuest;

	//We get the start node
	UQuestRuntimeNode** StartNodePtr = ChainQuest->ChainQuestGraph->Nodes.FindByPredicate([](const UQuestRuntimeNode* Node)
	{
		return Node->QuestNodeType ==  EQuestNodeType::StartNode;
	});

	UQuestRuntimeNode* a = *StartNodePtr;
	
	check(StartNodePtr);

	//We store the first quest node
	CurrentNode = a->OutputPins[0]->Connection->Parent;
}

void FChainQuestHandler::CheckCurrentNodeConditions(UQuestWorldSubsystem* QuestSystem)
{
	check(CurrentNode)

	UQuestInfo* Info = Cast<UQuestInfo>(CurrentNode->QuestInfo);

	TArray<TSubclassOf<UCondition>> Conditions;
	Info->OutPuts.GetKeys(Conditions);
	int Index = 0;
	for (const TSubclassOf<UCondition>& Condition : Conditions)
	{
		if (Condition->GetDefaultObject<UCondition>()->CheckCondition(QuestSystem))
		{
			if (CurrentNode->OutputPins[Index]->Connection->Parent->QuestNodeType == EQuestNodeType::QuestNode)
			{
				CurrentNode = CurrentNode->OutputPins[Index]->Connection->Parent;
				return;
			}
		}
		Index++;
	}
	
}
