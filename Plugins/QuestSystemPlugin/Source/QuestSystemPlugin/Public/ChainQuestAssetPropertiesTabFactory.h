#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FChainQuestEditorApp;

/*
 * The tab for the properties
 */
class FChainQuestAssetPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	FChainQuestAssetPropertiesTabFactory(TSharedPtr<FChainQuestEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FChainQuestEditorApp> App;
};
