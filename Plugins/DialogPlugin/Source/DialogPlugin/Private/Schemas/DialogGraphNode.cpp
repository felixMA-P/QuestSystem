#include "Schemas/DialogGraphNode.h"

#include "DialogLineInfo.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"

UDialogGraphNode::UDialogGraphNode() : UDialogGraphNodeBase()
{
	AddNewOutputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		DialogInfo->Outputs.Add(UDialogCondition::StaticClass(), FText::FromString("Output"));
		SyncPinsWithOutputs();
		GetGraph()->NotifyGraphChanged();
		GetGraph()->Modify();
	});

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		TArray<UEdGraphPin*> InputPins = Pins.FilterByPredicate([](UEdGraphPin* Pin)
		{
			return Pin->Direction == EGPD_Input;
		});

		const int CurrentNumberOfInputs = InputPins.Num() + 1;
		FString PinName = FString::Printf(TEXT("Input%d"), CurrentNumberOfInputs);

		CreateCustomPin(EGPD_Input, FName(PinName));

		GetGraph()->NotifyGraphChanged();
		GetGraph()->Modify();
	});

	DeleteOutputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		UEdGraphPin* PinToRemove = GetPinAt(Pins.Num() - 1);
		if (PinToRemove && PinToRemove->Direction != EEdGraphPinDirection::EGPD_Input)
		{
			RemovePin(PinToRemove);
			SyncPinsWithOutputs();
			GetGraph()->NotifyGraphChanged();
			GetGraph()->Modify();
		}
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

	DeleteNodeDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		GetGraph()->RemoveNode(this);
	});
}

void UDialogGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString("Dialog Node Actions"));

	Section.AddMenuEntry(
		TEXT("AddOutputPinEntry"),
		FText::FromString("Add Output Pin"),
		FText::FromString("Creates a new output pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeAddPinIcon")),
		FUIAction(AddNewOutputPinDelegate)
	);

	Section.AddMenuEntry(
		TEXT("AddInputPinEntry"),
		FText::FromString("Add Input Pin"),
		FText::FromString("Creates a new input pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeAddPinIcon")),
		FUIAction(AddNewInputPinDelegate)
	);

	Section.AddMenuEntry(
		TEXT("DeleteOutputPinEntry"),
		FText::FromString("Delete Output Pin"),
		FText::FromString("Deletes the last output pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeDeletePinIcon")),
		FUIAction(DeleteOutputPinDelegate)
	);

	Section.AddMenuEntry(
		TEXT("DeleteInputPinEntry"),
		FText::FromString("Delete Input Pin"),
		FText::FromString("Deletes the last input pin"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeDeletePinIcon")),
		FUIAction(DeleteInputPinDelegate)
	);

	Section.AddMenuEntry(
		TEXT("DeleteNode"),
		FText::FromString("Delete Node"),
		FText::FromString("Deletes the node"),
		FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeDeleteNodeIcon")),
		FUIAction(DeleteNodeDelegate)
	);
}

UEdGraphPin* UDialogGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category    = Direction == EEdGraphPinDirection::EGPD_Input ? TEXT("Inputs") : TEXT("Outputs");
	const FName SubCategory = TEXT("DialogPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

void UDialogGraphNode::SyncPinsWithOutputs()
{
	int NumOutputsInData = DialogInfo->Outputs.Num();

	TArray<TSubclassOf<UDialogCondition>> OutKeys;
	DialogInfo->Outputs.GetKeys(OutKeys);

	TArray<UEdGraphPin*> OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
	{
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	});

	int NumGraphPins = OutputPins.Num();

	while (NumGraphPins > NumOutputsInData)
	{
		RemovePin(OutputPins[NumGraphPins - 1]);
		NumGraphPins--;
	}
	while (NumOutputsInData > NumGraphPins)
	{
		CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(DialogInfo->Outputs[OutKeys[NumGraphPins]].ToString()));
		NumGraphPins++;
	}

	OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
	{
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	});

	int Index = 0;
	for (UEdGraphPin* OutputPin : OutputPins)
	{
		OutputPin->PinName = FName(DialogInfo->Outputs[OutKeys[Index]].ToString());
		Index++;
	}
}

UEdGraphPin* UDialogGraphNode::CreateDefaultInputPin()
{
	return CreateCustomPin(EGPD_Input, FName(TEXT("Input")));
}

void UDialogGraphNode::CreateDefaultOutputPins()
{
	Super::CreateDefaultOutputPins();
}

FText UDialogGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	check(DialogInfo);

	if (DialogInfo->DialogText.IsEmpty())
		return FText::FromString("Set up the dialog text");

	return DialogInfo->DialogText;
}
