#include "ChainQuestAssetAppMode.h"
#include "ChainQuestEditorApp.h"
#include "ChainQuestAssetPrimaryTabFactory.h"
#include "ChainQuestAssetPropertiesTabFactory.h"

FChainQuestAssetAppMode::FChainQuestAssetAppMode(TSharedPtr<FChainQuestEditorApp> InApp) : FApplicationMode(TEXT("ChainQuestAppMode"))
{
	App = InApp;
	
	TabSet.RegisterFactory(MakeShareable(new FChainQuestAssetPrimaryTabFactory(InApp)));
	TabSet.RegisterFactory(MakeShareable(new FChainQuestAssetPropertiesTabFactory(InApp)));
	
	TabLayout = FTabManager::NewLayout("ChainQuestAppMode_Layout_v3")->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)->Split(
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split(
				FTabManager::NewStack()
					->SetSizeCoefficient(0.75)
					->AddTab(InApp->PrimaryTabName, ETabState::OpenedTab)
				)
				->Split(
				FTabManager::NewStack()
					->SetSizeCoefficient(0.25)
					->AddTab(InApp->PropertiesTabName, ETabState::OpenedTab)
				)
		)
	);

	
}

void FChainQuestAssetAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FChainQuestEditorApp> LocalApp = App.Pin();
	LocalApp->PushTabFactories(TabSet);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FChainQuestAssetAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FChainQuestAssetAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
