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
class UChainQuestGraphSchema : public UEdGraphSchema
{

	GENERATED_BODY()
	
public:

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	
};
