#include "Schemas/ChainQuestGraphSchema.h"

#include "QuestInfo.h"
#include "QuestSystemPlugin.h"
#include "Schemas/QuestEndGraphNode.h"
#include "Schemas/QuestGraphNode.h"
#include "Schemas/QuestStartGraphNode.h"

void UChainQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewNodeAction> NewNodeAction(
		new FNewNodeAction(
			UQuestGraphNode::StaticClass(),
			FText::FromString("Nodes"),
			FText::FromString("New Quest Node"),
			FText::FromString("Makes a new node"),
			0
		));

	TSharedPtr<FNewNodeAction> NewEndNodeAction(
		new FNewNodeAction(
			UQuestEndGraphNode::StaticClass(),
			FText::FromString("Nodes"),
			FText::FromString("New End Node"),
			FText::FromString("Makes a new end node"),
			0
		));
	
	ContextMenuBuilder.AddAction(NewNodeAction);
	ContextMenuBuilder.AddAction(NewEndNodeAction);
}

const FPinConnectionResponse UChainQuestGraphSchema::CanCreateConnection(const UEdGraphPin* A,
	const UEdGraphPin* B) const
{
	if (A ==  nullptr || B == nullptr) FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need two pins"));
	
	if (A->Direction == B->Direction) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
	
	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
	
}

void UChainQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UQuestStartGraphNode* startNode = NewObject<UQuestStartGraphNode>(&Graph);
	startNode->CreateNewGuid();
	startNode->NodePosX = 0;
	startNode->NodePosY = 0;

	startNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(TEXT("Start")));
    
	Graph.AddNode(startNode, true, true);
	Graph.Modify();
	
}

UEdGraphNode* FNewNodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
                                            bool bSelectNewNode)
{
	UQuestGraphNodeBase* NewNode = NewObject<UQuestGraphNodeBase>(ParentGraph, ClassTemplate);
	NewNode->CreateNewGuid();
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	
	NewNode->InitNodeInfo(NewNode);

	UEdGraphPin* InputPin = NewNode->CreateDefaultInputPin();
	
	if (FromPin)
	{
		NewNode->GetSchema()->TryCreateConnection(FromPin,InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true, true);
	
	return NewNode;
}
