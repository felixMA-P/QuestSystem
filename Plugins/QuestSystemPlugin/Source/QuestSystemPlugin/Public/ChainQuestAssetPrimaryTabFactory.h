#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FChainQuestAssetEditorApp;

/*
 * The tab for the graph and nodes
 */
class FChainQuestAssetPrimaryTabFactory : public FWorkflowTabFactory
{
public:
	FChainQuestAssetPrimaryTabFactory(TSharedPtr<FChainQuestAssetEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FChainQuestAssetEditorApp> App;
};
