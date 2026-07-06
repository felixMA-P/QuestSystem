#include "Schemas/QuestKnotNode.h"

#include "SGraphNodeKnot.h"

UEdGraphPin* UQuestKnotNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category    = TEXT("QuestPin");
	const FName SubCategory = TEXT("QuestKnotPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

UEdGraphPin* UQuestKnotNode::CreateKnotInputPin()
{
	return CreateCustomPin(EEdGraphPinDirection::EGPD_Input, TEXT("KnotInput"));
}

UEdGraphPin* UQuestKnotNode::CreateKnotOutputPin()
{
	return CreateCustomPin(EEdGraphPinDirection::EGPD_Output, TEXT("KnotOutput"));
}

UEdGraphPin* UQuestKnotNode::GetPassThroughPin(const UEdGraphPin* FromPin) const
{
	if (FromPin == GetInputPin())
	{
		return GetOutputPin();
	}

	return GetInputPin();
}

bool UQuestKnotNode::ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
	OutInputPinIndex = 0;
	OutOutputPinIndex = 1;
	return true;
}

TSharedPtr<SGraphNode> UQuestKnotNode::CreateVisualWidget()
{
	return SNew(SGraphNodeKnot, this);
}
