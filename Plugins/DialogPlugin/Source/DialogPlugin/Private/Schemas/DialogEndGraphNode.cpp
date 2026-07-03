#include "Schemas/DialogEndGraphNode.h"

#include "Framework/Commands/UIAction.h"
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
		TArray<UEdGraphPin*> InputPins = Pins.FilterByPredicate([](UEdGraphPin* Pin)
		{
			return Pin->Direction == EGPD_Input;
		});

		const int CurrentNumberOfInputs = InputPins.Num();
		FString PinName = FString::Printf(TEXT("Input%d"), CurrentNumberOfInputs);

		CreateCustomPin(EGPD_Input, FName(PinName));

		GetGraph()->NotifyGraphChanged();
		GetGraph()->Modify();
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
			RemovePin(PinToRemove);
			GetGraph()->NotifyGraphChanged();
			GetGraph()->Modify();
		}
	});
}

FText UDialogEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!DialogInfo) return FText::FromString("End");

	return FText::FromString(DialogInfo->Title);
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
