#include "DialogAssetEditorApp.h"

#include "Dialog.h"
#include "DialogAssetAppMode.h"
#include "DialogGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Schemas/DialogGraphNodeBase.h"
#include "Schemas/DialogGraphNode.h"
#include "Schemas/DialogGraphSchema.h"
#include "Schemas/DialogStartGraphNode.h"
#include "Schemas/DialogEndGraphNode.h"
#include "DialogNodeType.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"

void FDialogAssetEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FDialogAssetEditorApp::InitEditor(const EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost>& InitToolKitHost, UObject* Object)
{
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(Object);

	WorkingAsset = Cast<UDialog>(Object);

	WorkingGraph = FBlueprintEditorUtils::CreateNewGraph(
		WorkingAsset,
		NAME_None,
		UEdGraph::StaticClass(),
		UDialogGraphSchema::StaticClass()
	);

	CreateGraphEditorCommands();

	InitAssetEditor(
		Mode,
		InitToolKitHost,
		GetToolkitFName(),
		FTabManager::FLayout::NullLayout,
		true,
		true,
		ObjectsToEdit
	);

	AddApplicationMode(AppModeName, MakeShareable(new FDialogAssetAppMode(SharedThis(this))));
	SetCurrentMode(AppModeName);

	UpdateEditorGraphFromWorkingAsset();

	GraphChangeListenerHandle = WorkingGraph->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &FDialogAssetEditorApp::OnGraphChanged));
}

void FDialogAssetEditorApp::OnClose()
{
	UpdateWorkingAssetFromGraph();
	WorkingGraph->RemoveOnGraphChangedHandler(GraphChangeListenerHandle);
	FWorkflowCentricApplication::OnClose();
}

void FDialogAssetEditorApp::SaveAsset_Execute()
{
	UpdateWorkingAssetFromGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void FDialogAssetEditorApp::OnGraphChanged(const FEdGraphEditAction& EditAction)
{
	UpdateWorkingAssetFromGraph();
}

void FDialogAssetEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if (WorkingGraphUI != nullptr)
	{
		UDialogGraphNodeBase* DialogGraphNode = GetSelectedNode(WorkingGraphUI->GetSelectedNodes());
		if (DialogGraphNode)
		{
			DialogGraphNode->OnPropertiesChanged();
		}
		WorkingGraphUI->NotifyGraphChanged();
	}
}

void FDialogAssetEditorApp::SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> InDetailsView)
{
	DetailsView = InDetailsView;
	DetailsView->OnFinishedChangingProperties().AddRaw(this, &FDialogAssetEditorApp::OnNodeDetailViewPropertiesUpdated);
}

void FDialogAssetEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection)
{
	UDialogGraphNodeBase* Node = GetSelectedNode(Selection);
	if (Node)
	{
		DetailsView->SetObject(Node->GetDialogInfoBase());
	}
	else
	{
		DetailsView->SetObject(nullptr);
	}
}

void FDialogAssetEditorApp::UpdateWorkingAssetFromGraph()
{
	if (WorkingAsset == nullptr || WorkingGraph == nullptr) return;

	UDialogGraph* RuntimeGraph = NewObject<UDialogGraph>(WorkingAsset);
	WorkingAsset->DialogGraph = RuntimeGraph;

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UDialogRuntimePin*> IdToPinMap;

	for (UEdGraphNode* UiNode : WorkingGraph->Nodes)
	{
		UDialogRuntimeNode* RuntimeNode = NewObject<UDialogRuntimeNode>(RuntimeGraph);
		RuntimeNode->Position = FVector2D(UiNode->NodePosX, UiNode->NodePosY);

		for (UEdGraphPin* UiPin : UiNode->Pins)
		{
			UDialogRuntimePin* RuntimePin = NewObject<UDialogRuntimePin>(RuntimeNode);
			RuntimePin->PinName = UiPin->PinName;
			RuntimePin->PinId   = UiPin->PinId;
			RuntimePin->Parent  = RuntimeNode;

			if (UiPin->HasAnyConnections() && UiPin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				Connections.Add(std::make_pair(UiPin->PinId, UiPin->LinkedTo[0]->PinId));
			}

			IdToPinMap.Add(UiPin->PinId, RuntimePin);

			if (UiPin->Direction == EEdGraphPinDirection::EGPD_Input)
				RuntimeNode->InputPins.Add(RuntimePin);
			else
				RuntimeNode->OutputPins.Add(RuntimePin);
		}

		UDialogGraphNodeBase* UIDialogNode = Cast<UDialogGraphNodeBase>(UiNode);
		RuntimeNode->DialogNodeType = UIDialogNode->GetDialogNodeType();
		RuntimeNode->DialogInfo     = DuplicateObject(UIDialogNode->GetDialogInfoBase(), RuntimeNode);

		RuntimeGraph->Nodes.Add(RuntimeNode);
	}

	for (std::pair<FGuid, FGuid> Connection : Connections)
	{
		UDialogRuntimePin* Pin1 = IdToPinMap[Connection.first];
		UDialogRuntimePin* Pin2 = IdToPinMap[Connection.second];
		Pin1->Connection = Pin2;
	}
}

void FDialogAssetEditorApp::UpdateEditorGraphFromWorkingAsset()
{
	if (WorkingAsset->DialogGraph == nullptr)
	{
		WorkingGraph->GetSchema()->CreateDefaultNodesForGraph(*WorkingGraph);
		return;
	}

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UEdGraphPin*> IdToPinMap;

	for (UDialogRuntimeNode* RuntimeNode : WorkingAsset->DialogGraph->Nodes)
	{
		UDialogGraphNodeBase* NewNode = nullptr;

		if      (RuntimeNode->DialogNodeType == EDialogNodeType::DialogNode) NewNode = NewObject<UDialogGraphNode>(WorkingGraph);
		else if (RuntimeNode->DialogNodeType == EDialogNodeType::StartNode)  NewNode = NewObject<UDialogStartGraphNode>(WorkingGraph);
		else if (RuntimeNode->DialogNodeType == EDialogNodeType::EndNode)    NewNode = NewObject<UDialogEndGraphNode>(WorkingGraph);

		check(NewNode);

		NewNode->CreateNewGuid();
		NewNode->NodePosX = RuntimeNode->Position.X;
		NewNode->NodePosY = RuntimeNode->Position.Y;

		if (RuntimeNode->DialogInfo != nullptr)
			NewNode->SetDialogInfo(DuplicateObject(RuntimeNode->DialogInfo, RuntimeNode));
		else
			NewNode->InitNodeInfo(NewNode);

		for (UDialogRuntimePin* Pin : RuntimeNode->InputPins)
		{
			UEdGraphPin* UiPin = NewNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Input, Pin->PinName);
			UiPin->PinId = Pin->PinId;
			IdToPinMap.Add(Pin->PinId, UiPin);
		}

		for (UDialogRuntimePin* Pin : RuntimeNode->OutputPins)
		{
			UEdGraphPin* UiPin = NewNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, Pin->PinName);
			UiPin->PinId = Pin->PinId;

			if (Pin->Connection != nullptr)
				Connections.Add(std::make_pair(Pin->PinId, Pin->Connection->PinId));

			IdToPinMap.Add(Pin->PinId, UiPin);
		}

		WorkingGraph->AddNode(NewNode, true, true);
	}

	for (std::pair<FGuid, FGuid> Connection : Connections)
	{
		UEdGraphPin* FromPin = IdToPinMap[Connection.first];
		UEdGraphPin* ToPin   = IdToPinMap[Connection.second];
		FromPin->LinkedTo.Add(ToPin);
		ToPin->LinkedTo.Add(FromPin);
	}
}

UDialogGraphNodeBase* FDialogAssetEditorApp::GetSelectedNode(const FGraphPanelSelectionSet& Selection)
{
	for (UObject* Object : Selection)
	{
		UDialogGraphNodeBase* Node = Cast<UDialogGraphNodeBase>(Object);
		if (Node != nullptr)
			return Node;
	}
	return nullptr;
}

void FDialogAssetEditorApp::CreateGraphEditorCommands()
{
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CanCopyNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CanCutNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CanPasteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CanDeleteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogAssetEditorApp::CanDuplicateNodes));
}

void FDialogAssetEditorApp::CopySelectedNodes()
{
	if (!WorkingGraphUI) return;
	FGraphPanelSelectionSet SelectedNodes = WorkingGraphUI->GetSelectedNodes();
	for (UObject* Obj : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(Obj))
			Node->PrepareForCopying();
	}
	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FDialogAssetEditorApp::CanCopyNodes() const
{
	return WorkingGraphUI && WorkingGraphUI->GetSelectedNodes().Num() > 0;
}

void FDialogAssetEditorApp::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

bool FDialogAssetEditorApp::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDialogAssetEditorApp::PasteNodes()
{
	if (!WorkingGraphUI || !WorkingGraph) return;

	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	WorkingGraph->Modify();
	WorkingGraphUI->ClearSelectionSet();

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(WorkingGraph, TextToImport, PastedNodes);
	
	// Regenerate GUIDS so in UpdateWorkingAssetFromGraph never has duplicate keys.
	for (UEdGraphNode* Node : PastedNodes)
	{
		for (UEdGraphPin* Pin : Node->Pins)
		{
			Pin->PinId = FGuid::NewGuid();
		}
		
		Node->NodeGuid = FGuid::NewGuid();
	}

	for (UEdGraphNode* Node : PastedNodes)
	{
		Node->NodePosX += 50;
		Node->NodePosY += 50;
		WorkingGraphUI->SetNodeSelection(Node, true);
	}

	WorkingGraphUI->NotifyGraphChanged();
}

bool FDialogAssetEditorApp::CanPasteNodes() const
{
	if (!WorkingGraph) return false;
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(WorkingGraph, ClipboardContent);
}

void FDialogAssetEditorApp::DeleteSelectedNodes()
{
	if (!WorkingGraphUI || !WorkingGraph) return;

	const FGraphPanelSelectionSet SelectedNodes = WorkingGraphUI->GetSelectedNodes();
	WorkingGraphUI->ClearSelectionSet();

	for (UObject* Obj : SelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(Obj);
		if (Node && Node->CanUserDeleteNode())
		{
			WorkingGraph->Modify();
			WorkingGraph->RemoveNode(Node);
		}
	}
}

bool FDialogAssetEditorApp::CanDeleteNodes() const
{
	if (!WorkingGraphUI) return false;
	const FGraphPanelSelectionSet SelectedNodes = WorkingGraphUI->GetSelectedNodes();
	for (UObject* Obj : SelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(Obj);
		if (Node && Node->CanUserDeleteNode())
			return true;
	}
	return false;
}

void FDialogAssetEditorApp::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDialogAssetEditorApp::CanDuplicateNodes() const
{
	return CanCopyNodes();
}
