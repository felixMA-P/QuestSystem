// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FDialogAssetEditorApp;

class FDialogAssetAppMode : public FApplicationMode
{
public:
	FDialogAssetAppMode(TSharedPtr<FDialogAssetEditorApp> InApp);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<FDialogAssetEditorApp> App;
	FWorkflowAllowedTabSet TabSet;
};
