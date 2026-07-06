#include "Schemas/DialogKnotNode.h"

#include "SGraphNodeKnot.h"

UEdGraphPin* UDialogKnotNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category    = TEXT("DialogPin");
	const FName SubCategory = TEXT("DialogKnotPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

UEdGraphPin* UDialogKnotNode::CreateKnotInputPin()
{
	return CreateCustomPin(EEdGraphPinDirection::EGPD_Input, TEXT("KnotInput"));
}

UEdGraphPin* UDialogKnotNode::CreateKnotOutputPin()
{
	return CreateCustomPin(EEdGraphPinDirection::EGPD_Output, TEXT("KnotOutput"));
}

UEdGraphPin* UDialogKnotNode::GetPassThroughPin(const UEdGraphPin* FromPin) const
{
	if (FromPin == GetInputPin())
	{
		return GetOutputPin();
	}

	return GetInputPin();
}

bool UDialogKnotNode::ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
	OutInputPinIndex = 0;
	OutOutputPinIndex = 1;
	return true;
}

TSharedPtr<SGraphNode> UDialogKnotNode::CreateVisualWidget()
{
	return SNew(SGraphNodeKnot, this);
}
