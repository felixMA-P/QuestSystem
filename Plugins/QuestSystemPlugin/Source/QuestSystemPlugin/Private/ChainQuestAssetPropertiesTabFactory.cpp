#include "ChainQuestAssetPropertiesTabFactory.h"
#include "ChainQuestEditorApp.h"
#include "ChainQuest.h"

FChainQuestAssetPropertiesTabFactory::FChainQuestAssetPropertiesTabFactory(TSharedPtr<FChainQuestEditorApp> InApp)
: FWorkflowTabFactory(InApp->PropertiesTabName, InApp)
{
	App = InApp;
	TabLabel = FText::FromString(TEXT("Primary"));
	ViewMenuDescription = FText::FromString(TEXT("Display a properties view for the current asset"));
	ViewMenuTooltip = FText::FromString(TEXT("Show de properties view"));
}

TSharedRef<SWidget> FChainQuestAssetPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	
	TSharedPtr<FChainQuestEditorApp> LocalApp = App.Pin();
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

	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					DetailsView.ToSharedRef()
				];
}

FText FChainQuestAssetPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A Properties view"));
	//return FWorkflowTabFactory::GetTabToolTipText(Info);
}
