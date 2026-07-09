#include "Schemas/QuestGraphNode.h"

#include "QuestInfo.h"
#include "Framework/Commands/UIAction.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"
#include "QuestInfo.h"

UQuestGraphNode::UQuestGraphNode() : UQuestGraphNodeBase()
{
	AddNewOutputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	const FScopedTransaction Transaction(FText::FromString("Add Quest Output Pin"));

	 	Modify();
	 	QuestInfo->Modify();

	 	FQuestOutput NewOutput;
	 	NewOutput.Condition = UCondition::StaticClass();
	 	NewOutput.Text = FText::FromString("Output");
	 	QuestInfo->OutPuts.Add(NewOutput);
	 	SyncPinsWithOutputs();
	 	GetGraph()->NotifyGraphChanged();
	 });

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	const FScopedTransaction Transaction(FText::FromString("Add Quest Input Pin"));

	 	Modify();

	 	CreateCustomPin(EGPD_Input, FName(TEXT("Input")));

	 	GetGraph()->NotifyGraphChanged();
	 });

	 DeleteOutputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	if (!QuestInfo || QuestInfo->OutPuts.IsEmpty()) return;

	 	const FScopedTransaction Transaction(FText::FromString("Delete Quest Output Pin"));

	 	Modify();
	 	QuestInfo->Modify();

	 	QuestInfo->OutPuts.Pop();

	 	SyncPinsWithOutputs();
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
	 		const FScopedTransaction Transaction(FText::FromString("Delete Quest Input Pin"));

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

void UQuestGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString("Custom Node Actions"));
	
	Section.AddMenuEntry(
		TEXT("AddOutputPinEntry"),
		FText::FromString("Add Output Pin"),
		FText::FromString("Creates a new pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeAddPinIcon")),
		FUIAction( AddNewOutputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("AddInputPinEntry"),
		FText::FromString("Add Input Pin"),
		FText::FromString("Creates a new pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeAddPinIcon")),
		FUIAction( AddNewInputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("DeleteOutputPinEntry"),
		FText::FromString("Delete Output Pin"),
		FText::FromString("Deletes the last output added pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeletePinIcon")),
		FUIAction( DeleteOutputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("DeleteInputPinEntry"),
		FText::FromString("Delete Input Pin"),
		FText::FromString("Deletes the last input added pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeletePinIcon")),
		FUIAction( DeleteInputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("DeleteNode"),
		FText::FromString("Delete Node"),
		FText::FromString("Deletes the node"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeleteNodeIcon")),
		FUIAction( DeleteNodeDelegate )
		);
}

UEdGraphPin* UQuestGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category = Direction == EEdGraphPinDirection::EGPD_Input ? TEXT("Inputs") : TEXT("Outputs");
	const FName SubCategory = TEXT("CustomPin");

	UEdGraphPin * Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}

void UQuestGraphNode::SyncPinsWithOutputs()
{
	if (!QuestInfo) return;

	int NumOfOutputPins = QuestInfo->OutPuts.Num();

	TArray<UEdGraphPin*> OutPutPins = GetAllPins().FilterByPredicate([](UEdGraphPin * Pin)
	 {
		 return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	 });

	int NumGraphNodePins = OutPutPins.Num();

	while (NumGraphNodePins > NumOfOutputPins)
	{
		RemovePin(OutPutPins[NumGraphNodePins - 1]);
		NumGraphNodePins--;
	}
	while (NumOfOutputPins > NumGraphNodePins)
	{
		CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName(QuestInfo->OutPuts[NumGraphNodePins].Text.ToString()));
		NumGraphNodePins++;
	}

	OutPutPins = GetAllPins().FilterByPredicate([](UEdGraphPin * Pin)
	 {
		 return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	 });

	int Index = 0;

	for (auto OutPutPin : OutPutPins)
	{
		OutPutPin->PinName = FName(QuestInfo->OutPuts[Index].Text.ToString());
		Index++;
	}

}

UEdGraphPin* UQuestGraphNode::CreateDefaultInputPin()
{
	return CreateCustomPin(EGPD_Input, FName(TEXT("Input")));
}

void UQuestGraphNode::CreateDefaultOutPutPins()
{
	Super::CreateDefaultOutPutPins();
}


FText UQuestGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!QuestInfo) return FText::FromString("Set up the title");

	if (QuestInfo->Title.IsEmpty())
		return FText::FromString("Set up the title");

	return QuestInfo->Title;
}

