// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogHandler.h"

#include "DialogEndInfo.h"
#include "DialogGraph.h"
#include "DialogLineInfo.h"
#include "DialogWorldSubsystem.h"
#include "Engine/World.h"

void UDialogHandler::Initialize(const UDialog* InDialog)
{
	if (!InDialog)
	{
		UE_LOG(LogTemp, Error, TEXT("Dialog System: UDialogHandler::Initialize called with null InDialog"));
		return;
	}

	if (!InDialog->DialogGraph)
	{
		UE_LOG(LogTemp, Error, TEXT("Dialog System: UDialogHandler::Initialize - Dialog '%s' its empty"), *InDialog->GetName());
		return;
	}

	Dialog = const_cast<UDialog*>(InDialog);

	UDialogRuntimeNode** StartNodePtr = Dialog->DialogGraph->Nodes.FindByPredicate([](const UDialogRuntimeNode* Node)
	{
		return Node && Node->DialogNodeType == EDialogNodeType::StartNode;
	});

	if (!StartNodePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Dialog System: UDialogHandler::Initialize - Dialog '%s' has no StartNode"), *InDialog->GetName());
		return;
	}

	UDialogRuntimeNode* StartNode = *StartNodePtr;

	if (StartNode->OutputPins.IsEmpty() || !StartNode->OutputPins[0]->Connection || !StartNode->OutputPins[0]->Connection->Parent)
	{
		UE_LOG(LogTemp, Error, TEXT("Dialog System: UDialogHandler::Initialize - Dialog '%s' StartNode has no connected output"), *InDialog->GetName());
		return;
	}

	CurrentNode = StartNode->OutputPins[0]->Connection->Parent;
}

bool UDialogHandler::SelectResponse(int32 ResponseIndex, UWorld* World)
{
	check(CurrentNode);
	check(CurrentNode->DialogNodeType == EDialogNodeType::DialogNode);

	if (!CurrentNode->OutputPins.IsValidIndex(ResponseIndex)) return false;
	if (!CurrentNode->OutputPins[ResponseIndex]->Connection) return false;

	// Fire the event associated with this response, if one is set
	UDialogLineInfo* LineInfo = Cast<UDialogLineInfo>(CurrentNode->DialogInfo);
	if (LineInfo && LineInfo->Outputs.IsValidIndex(ResponseIndex))
	{
		const FDialogOutput& Output = LineInfo->Outputs[ResponseIndex];
		if (Output.Event)
		{
			Output.Event->ExecuteEvent(World);
		}
	}

	// Advance to the connected node
	CurrentNode = CurrentNode->OutputPins[ResponseIndex]->Connection->Parent;

	return CurrentNode->DialogNodeType == EDialogNodeType::EndNode;
}

UDialogLineInfo* UDialogHandler::GetCurrentDialogLine() const
{
	if (!CurrentNode) return nullptr;

	return Cast<UDialogLineInfo>(CurrentNode->DialogInfo);
}
