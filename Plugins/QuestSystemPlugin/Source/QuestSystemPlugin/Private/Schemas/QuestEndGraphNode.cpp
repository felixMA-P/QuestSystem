#include "Schemas/QuestEndGraphNode.h"

#include <string>

UQuestEndGraphNode::UQuestEndGraphNode()
{
	DeleteNodeDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
		 GetGraph()->RemoveNode(this);
	 });

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	TArray<UEdGraphPin*> InputPins = Pins.FilterByPredicate([](UEdGraphPin* Pin)
	 	{
	 		return Pin->Direction == EGPD_Input;
	 	});

	    const int CurrentNumberOfInputs = InputPins.Num();
	 	FString CurrentInputName = TEXT("Input");
	 	FString NumberS = FString::FromInt(CurrentNumberOfInputs);
	 	CurrentInputName = CurrentInputName.Append(NumberS);
	 	
	 	CreateCustomPin(
			  EGPD_Input, 
			  FName(CurrentInputName)
			  );
		
	 	GetGraph()->NotifyGraphChanged();
	 	GetGraph()->Modify();
	 });
}

FText UQuestEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	check(QuestInfo);
	
	if (QuestInfo != nullptr)
	{
		return FText::FromString(QuestInfo->Title);
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

	Section.AddMenuEntry(
		TEXT("AddInputPinEntry"),
		FText::FromString("Add Input Pin"),
		FText::FromString("Creates a new pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeAddPinIcon")),
		FUIAction( AddNewInputPinDelegate )
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
	return CreateCustomPin(EGPD_Input, FName(TEXT("Input1")));
}


