// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FDialogAssetEditorApp;

class FDialogAssetPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	FDialogAssetPropertiesTabFactory(TSharedPtr<FDialogAssetEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FDialogAssetEditorApp> App;
};
