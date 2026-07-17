// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "Schemas/DialogStartGraphNode.h"

UEdGraphPin* UDialogStartGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category    = TEXT("Outputs");
	const FName SubCategory = TEXT("DialogStartPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}
