#include "DialogAssetAppMode.h"
#include "DialogAssetEditorApp.h"
#include "DialogAssetPrimaryTabFactory.h"
#include "DialogAssetPropertiesTabFactory.h"

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
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FDialogAssetAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FDialogAssetAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
