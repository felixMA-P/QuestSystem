#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestGraphNode.h"
#include "QuestStartGraphNode.generated.h"

UCLASS()
class UQuestStartGraphNode : public UQuestGraphNodeBase
{
	
	GENERATED_BODY()
	
public:

public: // UDialogStartGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString("Start"); }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Red); }
	virtual bool CanUserDeleteNode() const override { return false; }

public: // UDialogGraphNodeBase interface
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;

	virtual EQuestNodeType GetQuestNodeType() const override { return EQuestNodeType::StartNode; }
	
	
};
