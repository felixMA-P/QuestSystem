#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FChainQuestAssetEditorApp;

/*
 * This class creates set up the different Tabs that we want to create, this class is managed from the EditorApp
 */
class FChainQuestAssetAppMode : public FApplicationMode
{
public:
	FChainQuestAssetAppMode(TSharedPtr<FChainQuestAssetEditorApp> InApp);

	//This register the tab factories created in the constructor
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	
	TWeakPtr<FChainQuestAssetEditorApp> App;
	
	FWorkflowAllowedTabSet TabSet;

};
