#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "ChainQuestGraphSchema.generated.h"


USTRUCT()
struct FNewNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FNewNodeAction() {}

	FNewNodeAction(UClass* InClassTemplate, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGruping) :
	FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGruping), ClassTemplate(InClassTemplate) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	
protected:
	const UClass* ClassTemplate = nullptr;
};

UCLASS()
class UChainQuestGraphSchema : public UEdGraphSchema, public FEditorUndoClient
{

	GENERATED_BODY()
	
public:

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool IsConnectionRelinkingAllowed(UEdGraphPin* InPin) const override;

	virtual const FPinConnectionResponse CanRelinkConnectionToPin(const UEdGraphPin* OldSourcePin, const UEdGraphPin* TargetPinCandidate) const override;

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	virtual bool SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const override;

	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;

};
