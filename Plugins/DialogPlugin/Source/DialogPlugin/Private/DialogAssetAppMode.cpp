// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogAssetAppMode.h"
#include "DialogAssetEditorApp.h"
#include "DialogAssetPrimaryTabFactory.h"
#include "DialogAssetPropertiesTabFactory.h"
#include "Misc/EngineVersionComparison.h"

FDialogAssetAppMode::FDialogAssetAppMode(TSharedPtr<FDialogAssetEditorApp> InApp) : FApplicationMode(InApp->AppModeName)
{
	App = InApp;

	TabSet.RegisterFactory(MakeShareable(new FDialogAssetPrimaryTabFactory(InApp)));
	TabSet.RegisterFactory(MakeShareable(new FDialogAssetPropertiesTabFactory(InApp)));

	TabLayout = FTabManager::NewLayout("DialogAppMode_Layout_v1")->AddArea
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

void FDialogAssetAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FDialogAssetEditorApp> LocalApp = App.Pin();
	LocalApp->PushTabFactories(TabSet);
#if UE_VERSION_OLDER_THAN(5, 8, 0)
	FApplicationMode::RegisterTabFactories(InTabManager);
#else
	// 5.8 made the base method private; this is the sanctioned replacement.
	RegisterTabFactoriesWithAppAndManager(LocalApp.Get(), InTabManager.ToSharedRef());
#endif
}

void FDialogAssetAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FDialogAssetAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
