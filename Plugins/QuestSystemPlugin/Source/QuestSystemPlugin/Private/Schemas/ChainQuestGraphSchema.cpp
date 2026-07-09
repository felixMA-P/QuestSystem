#include "Schemas/ChainQuestGraphSchema.h"

#include "QuestInfo.h"
#include "QuestSystemPlugin.h"
#include "ScopedTransaction.h"
#include "Schemas/QuestEndGraphNode.h"
#include "Schemas/QuestGraphNode.h"
#include "Schemas/QuestGraphNodeBase.h"
#include "Schemas/QuestKnotNode.h"
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
	if (A == nullptr || B == nullptr) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need two pins"));
	
	if (A->Direction == B->Direction) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
	
	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
}

bool UChainQuestGraphSchema::IsConnectionRelinkingAllowed(UEdGraphPin* InPin) const
{
	return InPin != nullptr;
}

const FPinConnectionResponse UChainQuestGraphSchema::CanRelinkConnectionToPin(const UEdGraphPin* OldSourcePin,
	const UEdGraphPin* TargetPinCandidate) const
{
	return CanCreateConnection(OldSourcePin, TargetPinCandidate);
}

void UChainQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UQuestStartGraphNode* StartNode = NewObject<UQuestStartGraphNode>(&Graph, NAME_None, RF_Transactional);
	StartNode->CreateNewGuid();
	StartNode->NodePosX = 0;
	StartNode->NodePosY = 0;

	UEdGraphPin* StartOutputPin = StartNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(TEXT("Start")));
	UQuestGraphNodeBase::TagPinWithBranchColor(StartOutputPin, UQuestGraphNodeBase::GetBranchColor(0));

	Graph.AddNode(StartNode, true, true);
	Graph.Modify();
	
}

void UChainQuestGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(FText::FromString("Break Pin Links"));
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UChainQuestGraphSchema::OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const
{
	if (!PinA || !PinB) return;

	UEdGraph* Graph = PinA->GetOwningNode()->GetGraph();
	if (!Graph) return;

	const FScopedTransaction Transaction(FText::FromString("Add Reroute Node"));
	Graph->Modify();

	UQuestKnotNode* Knot = NewObject<UQuestKnotNode>(Graph, UQuestKnotNode::StaticClass(), NAME_None, RF_Transactional);
	Knot->CreateNewGuid();
	Knot->NodePosX = GraphPosition.X - 21;
	Knot->NodePosY = GraphPosition.Y - 12;
	Knot->CreateKnotInputPin();
	Knot->CreateKnotOutputPin();
	Graph->AddNode(Knot, true, true);

	UEdGraphPin* OutputSource = (PinA->Direction == EEdGraphPinDirection::EGPD_Output) ? PinA : PinB;
	UEdGraphPin* InputDest    = (PinA->Direction == EEdGraphPinDirection::EGPD_Output) ? PinB : PinA;

	OutputSource->BreakLinkTo(InputDest);
	TryCreateConnection(OutputSource, Knot->GetInputPin());
	TryCreateConnection(Knot->GetOutputPin(), InputDest);
}

FLinearColor UChainQuestGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinSubCategory == TEXT("QuestKnotPin"))
	{
		return FLinearColor::White;
	}

	FLinearColor BranchColor;
	if (UQuestGraphNodeBase::TryGetBranchColor(PinType, BranchColor))
	{
		return BranchColor;
	}

	// Unconnected input pins are untagged (see UQuestGraphNodeBase::SyncInputPinColors) and should read as neutral.
	if (PinType.PinSubCategory.IsNone())
	{
		return FLinearColor::White;
	}

	return Super::GetPinTypeColor(PinType);
}

bool UChainQuestGraphSchema::SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const
{
	if (Node == nullptr || Graph == nullptr || Node->GetGraph() != Graph)
	{
		return false;
	}
	
	Graph->RemoveNode(Node);
	
	Graph->Modify();
	return true;
}


UEdGraphNode* FNewNodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
                                            bool bSelectNewNode)
{
	const FScopedTransaction Transaction(FText::FromString("Add Quest Node"));

	UQuestGraphNodeBase* NewNode = NewObject<UQuestGraphNodeBase>(ParentGraph, ClassTemplate, NAME_None, RF_Transactional);
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
