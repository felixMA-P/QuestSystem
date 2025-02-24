#include "ChainQuestAssetAppMode.h"
#include "ChainQuestAssetEditorApp.h"
#include "ChainQuestAssetPrimaryTabFactory.h"
#include "ChainQuestAssetPropertiesTabFactory.h"

FChainQuestAssetAppMode::FChainQuestAssetAppMode(TSharedPtr<FChainQuestAssetEditorApp> InApp) : FApplicationMode(InApp->AppModeName)
{
	App = InApp;
	
	TabSet.RegisterFactory(MakeShareable(new FChainQuestAssetPrimaryTabFactory(InApp)));
	TabSet.RegisterFactory(MakeShareable(new FChainQuestAssetPropertiesTabFactory(InApp)));
	
	TabLayout = FTabManager::NewLayout("ChainQuestAppMode_Layout_v7")->AddArea
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
	TSharedPtr<FChainQuestAssetEditorApp> LocalApp = App.Pin();
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
