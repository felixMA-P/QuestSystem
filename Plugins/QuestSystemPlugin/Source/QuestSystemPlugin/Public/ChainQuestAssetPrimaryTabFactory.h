#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FChainQuestEditorApp;

/*
 * The tab for the graph and nodes
 */
class FChainQuestAssetPrimaryTabFactory : public FWorkflowTabFactory
{
public:
	FChainQuestAssetPrimaryTabFactory(TSharedPtr<FChainQuestEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FChainQuestEditorApp> App;
};
