#include "Schemas/QuestStartGraphNode.h"

UEdGraphPin* UQuestStartGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category = TEXT("Outputs");
	const FName SubCategory = TEXT("StartPin");

	UEdGraphPin * Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}
