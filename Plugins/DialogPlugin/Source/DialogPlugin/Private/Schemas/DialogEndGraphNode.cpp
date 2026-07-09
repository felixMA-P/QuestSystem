#include "Schemas/DialogEndGraphNode.h"

#include "Framework/Commands/UIAction.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"

UDialogEndGraphNode::UDialogEndGraphNode()
{
	DeleteNodeDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		GetGraph()->RemoveNode(this);
	});

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		const FScopedTransaction Transaction(FText::FromString("Add Dialog End Input Pin"));

		Modify();

		CreateCustomPin(EGPD_Input, FName(TEXT("Input")));

		GetGraph()->NotifyGraphChanged();
	});

	DeleteInputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		TArray<UEdGraphPin*> InputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
		{
			return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
		});

		if (InputPins.IsEmpty()) return;

		if (UEdGraphPin* PinToRemove = InputPins.Last())
		{
			const FScopedTransaction Transaction(FText::FromString("Delete Dialog End Input Pin"));

			Modify();

			RemovePin(PinToRemove);
			GetGraph()->NotifyGraphChanged();
		}
	});
}

FText UDialogEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!DialogInfo) return FText::FromString("End");

	return DialogInfo->Title.IsEmpty() ? FText::FromString("End") : DialogInfo->Title;
}

void UDialogEndGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString("Dialog End Node Actions"));

	Section.AddMenuEntry(
		TEXT("DeleteNode"),
		FText::FromString("Delete Node"),
		FText::FromString("Deletes the node"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeDeleteNodeIcon")),
		FUIAction(DeleteNodeDelegate)
	);

	Section.AddMenuEntry(
		TEXT("AddInputPinEntry"),
		FText::FromString("Add Input Pin"),
		FText::FromString("Creates a new input pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeAddPinIcon")),
		FUIAction(AddNewInputPinDelegate)
	);

	Section.AddMenuEntry(
		TEXT("DeleteInputPinEntry"),
		FText::FromString("Delete Input Pin"),
		FText::FromString("Deletes the last input pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeDeletePinIcon")),
		FUIAction(DeleteInputPinDelegate)
	);
}

UEdGraphPin* UDialogEndGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category    = TEXT("Inputs");
	const FName SubCategory = TEXT("DialogEndPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

UEdGraphPin* UDialogEndGraphNode::CreateDefaultInputPin()
{
	return CreateCustomPin(EGPD_Input, FName(TEXT("Input1")));
}
