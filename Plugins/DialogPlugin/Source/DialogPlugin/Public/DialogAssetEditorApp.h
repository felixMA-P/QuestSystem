#pragma once

#include "CoreMinimal.h"
#include "SGraphPanel.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UDialog;

class FDialogAssetEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook, public FEditorDelegates
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void InitEditor(const EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost>& InitToolKitHost, UObject* Object);

	UDialog* GetWorkingAsset() { return WorkingAsset; }
	UEdGraph* GetWorkingGraph() { return WorkingGraph; }
	TSharedPtr<FUICommandList> GetGraphEditorCommands() const { return GraphEditorCommands; }

	virtual FName GetToolkitFName() const override { return FName(TEXT("DialogEditorApp")); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString(TEXT("DialogEditorApp")); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("DialogEditorApp"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0.2f, 0.5f, 0.3f, 0.5f); }
	virtual FString GetDocumentationLink() const override { return TEXT("https://github.com/felixMA-P/QuestSystem"); }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}

	virtual void OnClose() override;
	virtual void SaveAsset_Execute() override;

	void OnGraphChanged(const FEdGraphEditAction& EditAction);
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event);

	void SetWorkingGraphUI(TSharedPtr<SGraphEditor> InWorkingGraphUI) { WorkingGraphUI = InWorkingGraphUI; }
	void SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> InDetailsView);

	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection);

	// Graph editor commands
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void PasteNodes();
	bool CanPasteNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;
	
	/*UNDO REDO*/
	
	virtual bool MatchesContext( const FTransactionContext& InContext, const TArray<TPair<UObject*, FTransactionObjectEvent>>& TransactionObjectContexts ) const  override;

	virtual void PostUndo( bool bSuccess ) override;
	
	virtual void PostRedo( bool bSuccess ) override;
	
	/*UNDO REDO*/

	FName PrimaryTabName   = FName(TEXT("DialogAssetPrimaryTab"));
	FName PropertiesTabName = FName(TEXT("DialogAssetPropertiesTab"));
	FName AppModeName       = FName(TEXT("DialogAssetAppMode"));

protected:
	virtual void UpdateWorkingAssetFromGraph();
	virtual void UpdateEditorGraphFromWorkingAsset();
	class UDialogGraphNodeBase* GetSelectedNode(const FGraphPanelSelectionSet& Selection);

private:
	void CreateGraphEditorCommands();

	// Re-derives every node's input pin colors from their current connections. Called whenever the graph topology changes.
	void SyncAllInputPinColors();

	UPROPERTY() UDialog* WorkingAsset = nullptr;
	UPROPERTY() UEdGraph* WorkingGraph = nullptr;

	TSharedPtr<SGraphEditor> WorkingGraphUI = nullptr;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<FUICommandList> GraphEditorCommands;

	FDelegateHandle GraphChangeListenerHandle;
};
