#include "Schemas/DialogGraphSchema.h"

#include "DialogPlugin.h"
#include "ScopedTransaction.h"
#include "Schemas/DialogEndGraphNode.h"
#include "Schemas/DialogGraphNode.h"
#include "Schemas/DialogStartGraphNode.h"

void UDialogGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewDialogNodeAction> NewNodeAction(
		new FNewDialogNodeAction(
			UDialogGraphNode::StaticClass(),
			FText::FromString("Nodes"),
			FText::FromString("New Dialog Node"),
			FText::FromString("Makes a new dialog node"),
			0
		));

	TSharedPtr<FNewDialogNodeAction> NewEndNodeAction(
		new FNewDialogNodeAction(
			UDialogEndGraphNode::StaticClass(),
			FText::FromString("Nodes"),
			FText::FromString("New End Node"),
			FText::FromString("Makes a new end node"),
			0
		));

	ContextMenuBuilder.AddAction(NewNodeAction);
	ContextMenuBuilder.AddAction(NewEndNodeAction);
}

const FPinConnectionResponse UDialogGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A == nullptr || B == nullptr) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need two pins"));

	if (A->Direction == B->Direction) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));

	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
}

bool UDialogGraphSchema::IsConnectionRelinkingAllowed(UEdGraphPin* InPin) const
{
	return InPin != nullptr;
}

const FPinConnectionResponse UDialogGraphSchema::CanRelinkConnectionToPin(const UEdGraphPin* OldSourcePin, const UEdGraphPin* TargetPinCandidate) const
{
	return CanCreateConnection(OldSourcePin, TargetPinCandidate);
}

void UDialogGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UDialogStartGraphNode* StartNode = NewObject<UDialogStartGraphNode>(&Graph, NAME_None, RF_Transactional);
	StartNode->CreateNewGuid();
	StartNode->NodePosX = 0;
	StartNode->NodePosY = 0;

	UEdGraphPin* StartOutputPin = StartNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(TEXT("Start")));

	Graph.AddNode(StartNode, true, true);

	UDialogEndGraphNode* EndNode = NewObject<UDialogEndGraphNode>(&Graph, NAME_None, RF_Transactional);
	EndNode->CreateNewGuid();
	EndNode->NodePosX = 300;
	EndNode->NodePosY = 0;
	EndNode->InitNodeInfo(EndNode);

	UEdGraphPin* EndInputPin = EndNode->CreateDefaultInputPin();

	Graph.AddNode(EndNode, true, true);

	TryCreateConnection(StartOutputPin, EndInputPin);

	Graph.Modify();
}

void UDialogGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(FText::FromString("Break Pin Links"));
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

bool UDialogGraphSchema::SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const
{
	if (Node == nullptr || Graph == nullptr || Node->GetGraph() != Graph)
		return false;

	Graph->RemoveNode(Node);
	Graph->Modify();
	return true;
}

UEdGraphNode* FNewDialogNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(FText::FromString("Add Dialog Node"));

	UDialogGraphNodeBase* NewNode = NewObject<UDialogGraphNodeBase>(ParentGraph, ClassTemplate, NAME_None, RF_Transactional);
	NewNode->CreateNewGuid();
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;

	NewNode->InitNodeInfo(NewNode);

	UEdGraphPin* InputPin = NewNode->CreateDefaultInputPin();

	if (FromPin)
	{
		NewNode->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true, true);

	return NewNode;
}
