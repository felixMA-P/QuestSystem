#include "ChainQuestAssetPrimaryTabFactory.h"
#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"
#include "GraphEditor.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

FChainQuestAssetPrimaryTabFactory::FChainQuestAssetPrimaryTabFactory(TSharedPtr<FChainQuestAssetEditorApp> InApp)
: FWorkflowTabFactory(InApp->PrimaryTabName, InApp)
{
	App = InApp;
	TabLabel = FText::FromString(TEXT("Primary"));
	ViewMenuDescription = FText::FromString(TEXT("Display a primary view"));
	ViewMenuTooltip = FText::FromString(TEXT("Show de primary view"));
}

TSharedRef<SWidget> FChainQuestAssetPrimaryTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FChainQuestAssetEditorApp> LocalApp = App.Pin();

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged.BindRaw(LocalApp.Get(), &FChainQuestAssetEditorApp::OnGraphSelectionChanged);

	TSharedPtr<SGraphEditor> GraphEditor =
		SNew(SGraphEditor)
		.GraphEvents(GraphEvents)
		.IsEditable(true)
		.GraphToEdit(LocalApp->GetWorkingGraph());
	
	LocalApp->SetWorkingGraphUI(GraphEditor);
	
	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					GraphEditor.ToSharedRef()
				];
}

FText FChainQuestAssetPrimaryTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Simple example"));
	//return FWorkflowTabFactory::GetTabToolTipText(Info);
}
