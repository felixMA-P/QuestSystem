#include "Schemas/QuestGraphNode.h"

#include "QuestInfo.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"
#include "QuestInfo.h"

UQuestGraphNode::UQuestGraphNode() : UQuestGraphNodeBase()
{
	AddNewOutputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	/*TODO*/
	 	SyncPinsWithOutputs();
	 	GetGraph()->NotifyGraphChanged();
	 	GetGraph()->Modify();
	 });

	AddNewInputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
		CreateCustomPin(
			 EGPD_Input, 
			 TEXT("Another Input")
			 );
		
		 GetGraph()->NotifyGraphChanged();
		 GetGraph()->Modify();
	 });

	 DeleteOutputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
	 	UEdGraphPin * PinToRemove = GetPinAt(Pins.Num() - 1);
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
	 	TArray<UEdGraphPin*> InputPins = GetAllPins().FilterByPredicate([](UEdGraphPin * Pin)
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
	int NumOfOutputPins = QuestInfo->OutPuts.Num();
	
	TArray<TSubclassOf<UCondition>> OutKeys;
	QuestInfo->OutPuts.GetKeys(OutKeys);

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
		CreateCustomPin(EEdGraphPinDirection::EGPD_Output, FName( QuestInfo->OutPuts[OutKeys[NumGraphNodePins]].ToString()));
		NumGraphNodePins++;
	}

	OutPutPins = GetAllPins().FilterByPredicate([](UEdGraphPin * Pin)
	 {
		 return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
	 });

	int Index = 0;
	
	for (auto OutPutPin : OutPutPins)
	{
		OutPutPin->PinName = FName(QuestInfo->OutPuts[OutKeys[Index]].ToString());
		Index++;
	}
	
}

UEdGraphPin* UQuestGraphNode::CreateDefaultInputPin()
{
	return CreateCustomPin(EGPD_Input, FName(TEXT("Display")));
}

void UQuestGraphNode::CreateDefaultOutPutPins()
{
	Super::CreateDefaultOutPutPins();
}


FText UQuestGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	check(QuestInfo);
	
	if (QuestInfo->Title.IsEmpty())
		return FText::FromString("Set up the title");
	
	return QuestInfo->Title;
}

