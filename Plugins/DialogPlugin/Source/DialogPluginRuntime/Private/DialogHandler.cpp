#include "DialogHandler.h"

#include "DialogEndInfo.h"
#include "DialogGraph.h"
#include "DialogLineInfo.h"
#include "DialogWorldSubsystem.h"

FDialogHandler::FDialogHandler(const UDialog* InDialog)
{
	Dialog = InDialog;

	UDialogRuntimeNode** StartNodePtr = Dialog->DialogGraph->Nodes.FindByPredicate([](const UDialogRuntimeNode* Node)
	{
		return Node->DialogNodeType == EDialogNodeType::StartNode;
	});

	check(StartNodePtr);

	UDialogRuntimeNode* StartNode = *StartNodePtr;
	CurrentNode = StartNode->OutputPins[0]->Connection->Parent;
	DialogInfos.Add(CurrentNode->DialogInfo);
}

bool FDialogHandler::CheckCurrentNodeConditions(UWorld* World)
{
	check(CurrentNode);

	UDialogLineInfo* Info = Cast<UDialogLineInfo>(CurrentNode->DialogInfo);
	if (!Info) return false;

	TArray<TSubclassOf<UDialogCondition>> Conditions;
	Info->Outputs.GetKeys(Conditions);
	int Index = 0;

	for (const TSubclassOf<UDialogCondition>& Condition : Conditions)
	{
		check(Condition);

		if (Condition.GetDefaultObject()->CheckCondition(World))
		{
			CurrentNode = CurrentNode->OutputPins[Index]->Connection->Parent;
			DialogInfos.Add(CurrentNode->DialogInfo);

			if (CurrentNode->DialogNodeType == EDialogNodeType::DialogNode)
			{
				return CheckCurrentNodeConditions(World);
			}

			if (CurrentNode->DialogNodeType == EDialogNodeType::EndNode)
			{
				return true;
			}
		}

		Index++;
	}

	return false;
}
