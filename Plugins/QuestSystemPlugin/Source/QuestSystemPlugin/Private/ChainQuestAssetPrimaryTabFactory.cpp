// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "ChainQuestAssetPrimaryTabFactory.h"
#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"
#include "GraphEditor.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Widgets/SNullWidget.h"

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
	if (!LocalApp.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged.BindRaw(LocalApp.Get(), &FChainQuestAssetEditorApp::OnGraphSelectionChanged);
	
	TSharedPtr<SGraphEditor> GraphEditor =
		SNew(SGraphEditor)
		.GraphEvents(GraphEvents)
		.IsEditable(true)
		.AdditionalCommands(LocalApp->GetGraphEditorCommands())
		.GraphToEdit(LocalApp->GetWorkingGraph());
	
	LocalApp->SetWorkingGraphUI(GraphEditor);

	FSlateApplication::Get().SetKeyboardFocus(GraphEditor, EFocusCause::SetDirectly);

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
