#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FChainQuestAssetEditorApp;

/*
 * The tab for the properties
 */
class FChainQuestAssetPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	FChainQuestAssetPropertiesTabFactory(TSharedPtr<FChainQuestAssetEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FChainQuestAssetEditorApp> App;
};
