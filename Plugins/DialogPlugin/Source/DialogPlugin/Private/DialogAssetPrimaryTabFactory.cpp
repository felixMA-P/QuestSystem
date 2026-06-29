#include "DialogAssetPrimaryTabFactory.h"
#include "DialogAssetEditorApp.h"
#include "GraphEditor.h"

FDialogAssetPrimaryTabFactory::FDialogAssetPrimaryTabFactory(TSharedPtr<FDialogAssetEditorApp> InApp)
	: FWorkflowTabFactory(InApp->PrimaryTabName, InApp)
{
	App = InApp;
	TabLabel = FText::FromString(TEXT("Dialog Graph"));
	ViewMenuDescription = FText::FromString(TEXT("Display the dialog graph view"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the dialog graph view"));
}

TSharedRef<SWidget> FDialogAssetPrimaryTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FDialogAssetEditorApp> LocalApp = App.Pin();

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged.BindRaw(LocalApp.Get(), &FDialogAssetEditorApp::OnGraphSelectionChanged);

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

FText FDialogAssetPrimaryTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Edit the dialog flow graph"));
}
