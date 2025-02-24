#include "ChainQuestAssetPropertiesTabFactory.h"
#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"

FChainQuestAssetPropertiesTabFactory::FChainQuestAssetPropertiesTabFactory(TSharedPtr<FChainQuestAssetEditorApp> InApp)
: FWorkflowTabFactory(InApp->PropertiesTabName, InApp)
{
	App = InApp;
	TabLabel = FText::FromString(TEXT("Properties"));
	ViewMenuDescription = FText::FromString(TEXT("Display a properties view for the current asset"));
	ViewMenuTooltip = FText::FromString(TEXT("Show de properties view"));
}

TSharedRef<SWidget> FChainQuestAssetPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	
	TSharedPtr<FChainQuestAssetEditorApp> LocalApp = App.Pin();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs detailsViewArgs;
	{
		detailsViewArgs.bAllowSearch = true;
		detailsViewArgs.bHideSelectionTip = true;
		detailsViewArgs.bLockable = false;
		detailsViewArgs.bSearchInitialKeyFocus = true;
		detailsViewArgs.bUpdatesFromSelection = false;
		detailsViewArgs.NotifyHook = nullptr;
		detailsViewArgs.bShowOptions = true;
		detailsViewArgs.bShowModifiedPropertiesOption = false;
		detailsViewArgs.bShowScrollBar = false;
	}

	TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(detailsViewArgs);
	DetailsView->SetObject(LocalApp->GetWorkingAsset());


	TSharedPtr<IDetailsView> SelectedNodeDetailsView = PropertyEditorModule.CreateDetailView(detailsViewArgs);
	SelectedNodeDetailsView->SetObject(nullptr);
	LocalApp->SetSelectedNodeDetailView(SelectedNodeDetailsView);
	
	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					DetailsView.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					SelectedNodeDetailsView.ToSharedRef()
				];
}

FText FChainQuestAssetPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A Properties view"));
	//return FWorkflowTabFactory::GetTabToolTipText(Info);
}
