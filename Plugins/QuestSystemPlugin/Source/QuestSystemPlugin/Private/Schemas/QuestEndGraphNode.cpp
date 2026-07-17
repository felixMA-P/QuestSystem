// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "Schemas/QuestEndGraphNode.h"

#include "ScopedTransaction.h"

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
	 	const FScopedTransaction Transaction(FText::FromString("Add Quest End Input Pin"));

	 	Modify();

	 	CreateCustomPin(EGPD_Input, FName(TEXT("Input")));

	 	GetGraph()->NotifyGraphChanged();
	 });

	DeleteInputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
		 TArray<UEdGraphPin*> InputPins = GetAllPins().FilterByPredicate([](UEdGraphPin * Pin)
		 {
			 return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
		 });

		 if (InputPins.IsEmpty()) return;

		if (UEdGraphPin* PinToRemove = InputPins.Last())
		{
			const FScopedTransaction Transaction(FText::FromString("Delete Quest End Input Pin"));

			Modify();

			RemovePin(PinToRemove);
			GetGraph()->NotifyGraphChanged();
		}
	});
}

FText UQuestEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!QuestInfo) return FText::FromString("End");

	return QuestInfo->Title.IsEmpty() ? FText::FromString("End") : QuestInfo->Title;
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

	Section.AddMenuEntry(
		TEXT("DeleteInputPinEntry"),
		FText::FromString("Delete Input Pin"),
		FText::FromString("Deletes the last input added pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeletePinIcon")),
		FUIAction( DeleteInputPinDelegate )
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


