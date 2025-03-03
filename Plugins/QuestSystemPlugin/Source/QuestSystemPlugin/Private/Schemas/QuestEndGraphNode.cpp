#include "Schemas/QuestEndGraphNode.h"

UQuestEndGraphNode::UQuestEndGraphNode()
{
	DeleteNodeDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
		 GetGraph()->RemoveNode(this);
	 });
}

FText UQuestEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	check(QuestInfo);
	
	if (QuestInfo != nullptr)
	{
		return FText::FromString(QuestInfo->ActionData);
	}

	return FText::FromString("");
}

void UQuestEndGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString("Custom Node Actions"));

	Section.AddMenuEntry(
		TEXT("DeleteNode"),
		FText::FromString("Delete Node"),
		FText::FromString("Deletes the node"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeleteNodeIcon")),
		FUIAction( DeleteNodeDelegate )
		);
}

UEdGraphPin* UQuestEndGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category = TEXT("Inputs");
	const FName SubCategory = TEXT("EndPin");

	UEdGraphPin * Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

UEdGraphPin* UQuestEndGraphNode::CreateDefaultInputPin()
{
	return CreateCustomPin(EGPD_Input, FName(TEXT("Input")));
}


