#include "Schemas/CustomGraphNode.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"

UCustomGraphNode::UCustomGraphNode() : UEdGraphNode()
{
	AddNewOutputPinDelegate = FExecuteAction::CreateLambda(
	 [this]()
	 {
		CreateCustomPin(
			 EGPD_Output, 
			 TEXT("Another Output")
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

void UCustomGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString("Custom Node Actions"));
	
	Section.AddMenuEntry(
		TEXT("AddPinEntry"),
		FText::FromString("Add Pin"),
		FText::FromString("Creates a new pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("PQuestEditor.NodeAddinIcon")),
		FUIAction( AddNewOutputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("DeletePinEntry"),
		FText::FromString("Delete Pin"),
		FText::FromString("Deletes the last added pin"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeletePinIcon")),
		FUIAction( DeleteOutputPinDelegate )
		);

	Section.AddMenuEntry(
		TEXT("DeleteNode"),
		FText::FromString("Delete Node"),
		FText::FromString("Deletes the node"),
		FSlateIcon(TEXT("QuestSystemEditorStyle"), TEXT("QuestEditor.NodeDeleteNodeIcon")),
		FUIAction( DeleteNodeDelegate )
		);
}

UEdGraphPin* UCustomGraphNode::CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name)
{
	const FName Category = Direction == EEdGraphPinDirection::EGPD_Input ? TEXT("Inputs") : TEXT("Outputs");
	const FName SubCategory = TEXT("CustomPin");

	UEdGraphPin * Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = SubCategory;

	return Pin;
}


