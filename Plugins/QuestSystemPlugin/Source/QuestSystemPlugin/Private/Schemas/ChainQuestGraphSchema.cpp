#include "Schemas/ChainQuestGraphSchema.h"

#include "QuestInfo.h"
#include "QuestSystemPlugin.h"
#include "Schemas/QuestGraphNode.h"

void UChainQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewNodeAction> NewNodeAction(
		new FNewNodeAction(
			FText::FromString("Nodes"),
			FText::FromString("NewNodeAction"),
			FText::FromString("Makes a new node"),
			0
		));
	
	ContextMenuBuilder.AddAction(NewNodeAction);
}

const FPinConnectionResponse UChainQuestGraphSchema::CanCreateConnection(const UEdGraphPin* A,
	const UEdGraphPin* B) const
{
	if (A ==  nullptr || B == nullptr) FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need two pins"));
	
	if (A->Direction == B->Direction) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
	
	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
	
}

UEdGraphNode* FNewNodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
                                            bool bSelectNewNode)
{
	UQuestGraphNode* NewNode = NewObject<UQuestGraphNode>(ParentGraph);
	NewNode->CreateNewGuid();
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;

	NewNode->SetQuestInfo(NewObject<UQuestInfo>(NewNode));

	UEdGraphPin* InputPin = NewNode->CreateCustomPin(EGPD_Input, TEXT("Input1"));

	NewNode->SyncPinsWithOutputs();

	if (FromPin)
	{
		NewNode->GetSchema()->TryCreateConnection(FromPin,InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true, true);
	
	return NewNode;
}
