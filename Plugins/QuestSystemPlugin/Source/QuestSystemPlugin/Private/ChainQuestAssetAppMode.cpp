// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "ChainQuestAssetAppMode.h"
#include "ChainQuestAssetEditorApp.h"
#include "ChainQuestAssetPrimaryTabFactory.h"
#include "ChainQuestAssetPropertiesTabFactory.h"
#include "Misc/EngineVersionComparison.h"

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
#if UE_VERSION_OLDER_THAN(5, 8, 0)
	FApplicationMode::RegisterTabFactories(InTabManager);
#else
	// 5.8 made the base method private; this is the sanctioned replacement.
	RegisterTabFactoriesWithAppAndManager(LocalApp.Get(), InTabManager.ToSharedRef());
#endif
}

void FChainQuestAssetAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FChainQuestAssetAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
