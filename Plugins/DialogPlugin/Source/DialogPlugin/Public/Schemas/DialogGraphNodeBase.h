// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogNodeType.h"
#include "DialogInfoBase.h"
#include "DialogGraphNodeBase.generated.h"

UCLASS()
class UDialogGraphNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) { return nullptr; }
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutputPins() {}

	virtual void InitNodeInfo(UObject* Outer) {}
	virtual void SetDialogInfo(UDialogInfoBase* Info) {}
	virtual UDialogInfoBase* GetDialogInfoBase() const { return nullptr; }

	virtual EDialogNodeType GetDialogNodeType() const { return EDialogNodeType::Unknown; }

	virtual void OnPropertiesChanged() {}

	// Default color offered to a newly added output, cycling through a fixed palette by index. Purely a starting point;
	// the authoritative color for an existing output is whatever is stored in FDialogOutput::Color.
	static FLinearColor GetBranchColor(int32 Index);

	// Tags Pin with Color so it can be recovered later by TryGetBranchColor (used by the schema to color the pin/wire).
	static void TagPinWithBranchColor(UEdGraphPin* Pin, const FLinearColor& Color);

	// Extracts the color a pin was tagged with via TagPinWithBranchColor, if any.
	static bool TryGetBranchColor(const FEdGraphPinType& PinType, FLinearColor& OutColor);

	// Re-derives each input pin's color tag from whatever it's currently connected to (walking through any reroute/
	// knot nodes to the real output pin), or clears the tag if unconnected. Call after any graph topology change.
	void SyncInputPinColors();
};
