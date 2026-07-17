// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogAssetPropertiesTabFactory.h"
#include "DialogAssetEditorApp.h"
#include "Dialog.h"

FDialogAssetPropertiesTabFactory::FDialogAssetPropertiesTabFactory(TSharedPtr<FDialogAssetEditorApp> InApp)
	: FWorkflowTabFactory(InApp->PropertiesTabName, InApp)
{
	App = InApp;
	TabLabel = FText::FromString(TEXT("Properties"));
	ViewMenuDescription = FText::FromString(TEXT("Display properties for the current asset and selected node"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the properties view"));
}

TSharedRef<SWidget> FDialogAssetPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FDialogAssetEditorApp> LocalApp = App.Pin();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.NotifyHook = nullptr;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = false;
	}

	TSharedPtr<IDetailsView> AssetDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	AssetDetailsView->SetObject(LocalApp->GetWorkingAsset());

	TSharedPtr<IDetailsView> NodeDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	NodeDetailsView->SetObject(nullptr);
	LocalApp->SetSelectedNodeDetailView(NodeDetailsView);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			AssetDetailsView.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			NodeDetailsView.ToSharedRef()
		];
}

FText FDialogAssetPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A properties view for the dialog asset and selected node"));
}
