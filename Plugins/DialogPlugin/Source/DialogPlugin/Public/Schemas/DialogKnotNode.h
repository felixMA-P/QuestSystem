// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogGraphNodeBase.h"
#include "DialogKnotNode.generated.h"

UCLASS()
class UDialogKnotNode : public UDialogGraphNodeBase
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

	virtual EDialogNodeType GetDialogNodeType() const override { return EDialogNodeType::KnotNode; }

	virtual bool IsCompilerRelevant() const override { return false; }
	virtual UEdGraphPin* GetPassThroughPin(const UEdGraphPin* FromPin) const override;
	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override;

	virtual bool ShouldOverridePinNames() const override { return true; }
	virtual FText GetPinNameOverride(const UEdGraphPin& Pin) const override { return FText::GetEmpty(); }

	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	
};
