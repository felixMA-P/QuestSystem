#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"


class UChainQuest;

/*
 * Initializes the data for the asset and serves as a connection between the interface (@WorkingGraph) and the data (@WorkingAsset)
 */
class FChainQuestEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{

public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	// this is the most important function, it initializes the editor for the asset
	void InitEditor(const EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost> & InitToolKitHost, UObject* Object);

	
	UChainQuest* GetWorkingAsset() { return WorkingAsset; }
	UEdGraph* GetWorkingGraph() { return WorkingGraph; }
	
	// FAssetEditorToolKit
	virtual FName GetToolkitFName() const override { return FName(TEXT("ChainQuestEditorApp")); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString(TEXT("ChainQuestEditorApp")); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("ChainQuestEditorApp"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f); }
	virtual FString GetDocumentationLink() const override { return TEXT("GIT URL"); }

	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}

	FName PrimaryTabName = FName(TEXT("ChainQuestAssetPrimaryTab"));
	FName PropertiesTabName = FName(TEXT("ChainQuestAssetPropertiesTab"));

private:
	UPROPERTY()
	UChainQuest* WorkingAsset = nullptr;

	UPROPERTY()
	UEdGraph* WorkingGraph = nullptr;
};
