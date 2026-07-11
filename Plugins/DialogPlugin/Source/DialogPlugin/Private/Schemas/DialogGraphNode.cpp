#include "Schemas/DialogGraphNode.h"

#include "DialogLineInfo.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Framework/Commands/UIAction.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"

namespace
{
	// Word-wraps at the nearest space at/after MaxLineLength characters; MaxLineLength <= 0 disables wrapping.
	FText WrapTextAtLength(const FText& InTextResume, const FText& InText, int32 MaxLineLength)
	{
		if (MaxLineLength <= 0)
			return InText;

		TArray<FString> Words;
		InText.ToString().ParseIntoArrayWS(Words);
		
		FString Result;
		FString CurrentLine;
		
		Result += InTextResume.ToString() + LINE_TERMINATOR;

		for (const FString& Word : Words)
		{
			const FString Candidate = CurrentLine.IsEmpty() ? Word : CurrentLine + TEXT(" ") + Word;

			if (Candidate.Len() > MaxLineLength && !CurrentLine.IsEmpty())
			{
				Result += CurrentLine + LINE_TERMINATOR;
				CurrentLine = Word;
			}
			else
			{
				CurrentLine = Candidate;
			}
		}

		if (!CurrentLine.IsEmpty())
			Result += CurrentLine;

		return FText::FromString(Result);
	}
}

UDialogGraphNode::UDialogGraphNode() : UDialogGraphNodeBase()
{
	AddNewOutputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		if (!DialogInfo) return;

		const FScopedTransaction Transaction(FText::FromString("Add Dialog Output Pin"));

		Modify();
		DialogInfo->Modify();

		FDialogOutput NewOutput;
		NewOutput.ResponseText = FText::FromString(TEXT("Response"));
		DialogInfo->Outputs.Add(NewOutput);
		SyncPinsWithOutputs();
		GetGraph()->NotifyGraphChanged();
	});

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		const FScopedTransaction Transaction(FText::FromString("Add Dialog Input Pin"));

		Modify();

		CreateCustomPin(EGPD_Input, FName(TEXT("Input")));

		GetGraph()->NotifyGraphChanged();
	});

	DeleteOutputPinDelegate = FExecuteAction::CreateLambda(
	[this]()
	{
		UEdGraphPin* PinToRemove = GetPinAt(Pins.Num() - 1);
		if (PinToRemove && PinToRemove->Direction != EEdGraphPinDirection::EGPD_Input)
		{
			const FScopedTransaction Transaction(FText::FromString("Delete Dialog Output Pin"));

			Modify();
			if (DialogInfo)
			{
				DialogInfo->Modify();
			}

			RemovePin(PinToRemove);
			if (DialogInfo && DialogInfo->Outputs.Num() > 0)
			{
				DialogInfo->Outputs.RemoveAt(DialogInfo->Outputs.Num() - 1);
			}
			GetGraph()->NotifyGraphChanged();
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
			const FScopedTransaction Transaction(FText::FromString("Delete Dialog Input Pin"));

			Modify();

			RemovePin(PinToRemove);
			GetGraph()->NotifyGraphChanged();
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
		FText::FromString("Add Response Pin"),
		FText::FromString("Creates a new player response output"),
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
		FText::FromString("Delete Response Pin"),
		FText::FromString("Deletes the last response output"),
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

	if (Context->Pin && Context->Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		TArray<UEdGraphPin*> OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
		{
			return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
		});

		const int32 PinIndex = OutputPins.IndexOfByKey(Context->Pin);

		if (DialogInfo && DialogInfo->Outputs.IsValidIndex(PinIndex))
		{
			FToolMenuSection& PinSection = Menu->AddSection(TEXT("PinSectionName"), FText::FromString("Pin Actions"));

			UDialogGraphNode* MutableThis = const_cast<UDialogGraphNode*>(this);

			PinSection.AddMenuEntry(
				TEXT("SetPinColorEntry"),
				FText::FromString("Set Pin Color"),
				FText::FromString("Choose the color for this output pin and its wire"),
				FSlateIcon(TEXT("DialogSystemEditorStyle"), TEXT("DialogEditor.NodeAddPinIcon")),
				FUIAction(FExecuteAction::CreateUObject(MutableThis, &UDialogGraphNode::OpenPinColorPicker, PinIndex))
			);
		}
	}
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
	if (!DialogInfo) return;

	const int NumOutputsInData = DialogInfo->Outputs.Num();

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
		UEdGraphPin* NewPin = CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(FString::Printf(TEXT("Output_%d"), NumGraphPins)));
		TagPinWithBranchColor(NewPin, GetBranchColor(NumGraphPins));
		NumGraphPins++;
	}

	// Refresh pin display labels from ResponseText; PinName stays stable
	OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
	{
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	});

	for (int32 Index = 0; Index < OutputPins.Num(); ++Index)
	{
		OutputPins[Index]->PinFriendlyName = DialogInfo->Outputs[Index].ResponseText;
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
	if (!DialogInfo)
		return FText::FromString("Set up the dialog text");

	if (DialogInfo->DialogText.IsEmpty())
		return FText::FromString("Set up the dialog text");

	return WrapTextAtLength(DialogInfo->DialogResume, DialogInfo->DialogText, DialogInfo->GraphPreviewWrapLength);
}

void UDialogGraphNode::OpenPinColorPicker(int32 OutputIndex)
{
	TArray<UEdGraphPin*> OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
	{
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	});

	if (!OutputPins.IsValidIndex(OutputIndex)) return;

	FLinearColor CurrentColor;
	if (!TryGetBranchColor(OutputPins[OutputIndex]->PinType, CurrentColor))
	{
		CurrentColor = GetBranchColor(OutputIndex);
	}

	FColorPickerArgs PickerArgs;
	PickerArgs.bUseAlpha = false;
	PickerArgs.bOnlyRefreshOnMouseUp = false;
	PickerArgs.bOnlyRefreshOnOk = false;
	PickerArgs.InitialColor = CurrentColor;
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UDialogGraphNode::HandlePinColorPicked, OutputIndex);

	OpenColorPicker(PickerArgs);
}

void UDialogGraphNode::HandlePinColorPicked(FLinearColor NewColor, int32 OutputIndex)
{
	TArray<UEdGraphPin*> OutputPins = GetAllPins().FilterByPredicate([](UEdGraphPin* Pin)
	{
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	});

	if (!OutputPins.IsValidIndex(OutputIndex)) return;

	const FScopedTransaction Transaction(FText::FromString("Set Dialog Pin Color"));

	Modify();
	TagPinWithBranchColor(OutputPins[OutputIndex], NewColor);

	GetGraph()->NotifyGraphChanged();
}
