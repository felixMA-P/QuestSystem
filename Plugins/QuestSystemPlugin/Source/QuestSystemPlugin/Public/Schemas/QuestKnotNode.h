#pragma once

#include "CoreMinimal.h"
#include "QuestGraphNodeBase.h"
#include "QuestKnotNode.generated.h"

UCLASS()
class UQuestKnotNode : public UQuestGraphNodeBase
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::GetEmpty(); }
	virtual bool CanUserDeleteNode() const override { return true; }

	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;
	UEdGraphPin* CreateKnotInputPin();
	UEdGraphPin* CreateKnotOutputPin();

	UEdGraphPin* GetInputPin() const { return Pins.IsValidIndex(0) ? Pins[0] : nullptr; }
	UEdGraphPin* GetOutputPin() const { return Pins.IsValidIndex(1) ? Pins[1] : nullptr; }

	virtual EQuestNodeType GetQuestNodeType() const override { return EQuestNodeType::KnotNode; }

	virtual bool IsCompilerRelevant() const override { return false; }
	virtual UEdGraphPin* GetPassThroughPin(const UEdGraphPin* FromPin) const override;
	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override;

	virtual bool ShouldOverridePinNames() const override { return true; }
	virtual FText GetPinNameOverride(const UEdGraphPin& Pin) const override { return FText::GetEmpty(); }

	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
};
