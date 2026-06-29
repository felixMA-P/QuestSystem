#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"
#include "ChainQuestAssetAppMode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Schemas/ChainQuestGraphSchema.h"
#include "ChainQuestGraph.h"
#include "Schemas/QuestGraphNode.h"
#include "QuestInfo.h"
#include "Schemas/QuestStartGraphNode.h"
#include "Schemas/QuestEndGraphNode.h"
#include "QuestNodeType.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"

void FChainQuestAssetEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
	
}

void FChainQuestAssetEditorApp::InitEditor(const EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost>& InitToolKitHost,
	UObject* Object)
{
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(Object);

	WorkingAsset = Cast<UChainQuest>(Object);

	CreateGraphEditorCommands();

	WorkingGraph = FBlueprintEditorUtils::CreateNewGraph(
		WorkingAsset,
		NAME_None,
		UEdGraph::StaticClass(),
		UChainQuestGraphSchema::StaticClass()
	);

	InitAssetEditor(
		Mode,
		InitToolKitHost,
		GetToolkitFName(),
		FTabManager::FLayout::NullLayout,
		true, //create standalane menu
		true, //create default tool bar
		ObjectsToEdit
		);

	//initializes the app mode
	AddApplicationMode(AppModeName, MakeShareable(new FChainQuestAssetAppMode(SharedThis(this))));
	SetCurrentMode(AppModeName);

	UpdateEditorGraphFromWorkingAsset();

	GraphChangeListenerHandle = WorkingGraph->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &FChainQuestAssetEditorApp::OnGraphChanged));
}

void FChainQuestAssetEditorApp::OnClose()
{
	UpdateWorkingAssetFromGraph();
	WorkingGraph->RemoveOnGraphChangedHandler(GraphChangeListenerHandle);
	FWorkflowCentricApplication::OnClose();
}

void FChainQuestAssetEditorApp::SaveAsset_Execute()
{
	UpdateWorkingAssetFromGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}


void FChainQuestAssetEditorApp::OnGraphChanged(const FEdGraphEditAction& EditAction)
{
	UpdateWorkingAssetFromGraph();
}

void FChainQuestAssetEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if(WorkingGraphUI != nullptr)
	{
		UQuestGraphNodeBase* QuestGraphNode = GetSelectedNode(WorkingGraphUI->GetSelectedNodes());
		if(QuestGraphNode)
		{
			QuestGraphNode->OnPropertiesChanged();
		} 
		WorkingGraphUI->NotifyGraphChanged();
	}
}

void FChainQuestAssetEditorApp::SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> InDetailsView)
{
	DetailsView = InDetailsView;
	DetailsView->OnFinishedChangingProperties().AddRaw(this, &FChainQuestAssetEditorApp::OnNodeDetailViewPropertiesUpdated);
}

void FChainQuestAssetEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection)
{
	if (!DetailsView) return;

	UQuestGraphNodeBase* Node = GetSelectedNode(Selection);
	if (Node)
	{
		DetailsView->SetObject(Node->GetQuestInfoBase());
	}
	else
	{
		DetailsView->SetObject(nullptr);
	}
}

void FChainQuestAssetEditorApp::UpdateWorkingAssetFromGraph()
{
	if (WorkingAsset == nullptr || WorkingGraph == nullptr) return;

	UChainQuestGraph* RuntimeGraph = NewObject<UChainQuestGraph>(WorkingAsset);
	WorkingAsset->ChainQuestGraph = RuntimeGraph;
	
	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UQuestRuntimePin*> IdToPinMap;

	for (UEdGraphNode* UiNode : WorkingGraph->Nodes)
	{
		UQuestRuntimeNode* RuntimeNode = NewObject<UQuestRuntimeNode>(RuntimeGraph);
		RuntimeNode->Position = FVector2D(UiNode->NodePosX, UiNode->NodePosY);
		
		for (UEdGraphPin* UiPin : UiNode->Pins)
		{
			UQuestRuntimePin* RuntimePin = NewObject<UQuestRuntimePin>(RuntimeNode);
			RuntimePin->PinName = UiPin->PinName;
			RuntimePin->PinId = UiPin->PinId;
			RuntimePin->Parent = RuntimeNode;
			
			if (UiPin->HasAnyConnections() && UiPin->Direction == EEdGraphPinDirection::EGPD_Output) {
				std::pair<FGuid, FGuid> Connection = std::make_pair(UiPin->PinId, UiPin->LinkedTo[0]->PinId);
				Connections.Add(Connection);
			}
			
			IdToPinMap.Add(UiPin->PinId, RuntimePin);
			
			if (UiPin->Direction == EEdGraphPinDirection::EGPD_Input) {
				RuntimeNode->InputPins.Add(RuntimePin);
			} else {
				RuntimeNode->OutputPins.Add(RuntimePin);
			}
		}
		
		UQuestGraphNodeBase* UIQuestGraphNode = Cast<UQuestGraphNodeBase>(UiNode);
		RuntimeNode->QuestNodeType = UIQuestGraphNode->GetQuestNodeType();
		RuntimeNode->QuestInfo = DuplicateObject(UIQuestGraphNode->GetQuestInfoBase(), RuntimeNode);
		
		RuntimeGraph->Nodes.Add(RuntimeNode);
	}

	for (std::pair<FGuid, FGuid> Connection : Connections)
	{
		UQuestRuntimePin* pin1 = IdToPinMap[Connection.first];
		UQuestRuntimePin* pin2 = IdToPinMap[Connection.second];
		pin1->Connection = pin2;
	}
	
}

void FChainQuestAssetEditorApp::UpdateEditorGraphFromWorkingAsset()
{
	if (WorkingAsset->ChainQuestGraph == nullptr)
	{
		WorkingGraph->GetSchema()->CreateDefaultNodesForGraph(*WorkingGraph);
		return;
	}

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UEdGraphPin*> IdToPinMap;

	for (UQuestRuntimeNode* RuntimeNode : WorkingAsset->ChainQuestGraph->Nodes)
	{

		UQuestGraphNodeBase* NewNode = nullptr;
		if (RuntimeNode->QuestNodeType == EQuestNodeType::QuestNode)
			NewNode = NewObject<UQuestGraphNode>(WorkingGraph);
		else if (RuntimeNode->QuestNodeType == EQuestNodeType::StartNode)
			NewNode = NewObject<UQuestStartGraphNode>(WorkingGraph);
		else if (RuntimeNode->QuestNodeType == EQuestNodeType::EndNode)
			NewNode = NewObject<UQuestEndGraphNode>(WorkingGraph);

		check(NewNode);
		
		NewNode->CreateNewGuid();
		NewNode->NodePosX = RuntimeNode->Position.X;
		NewNode->NodePosY = RuntimeNode->Position.Y;

		if (RuntimeNode->QuestInfo != nullptr)
		{
			NewNode->SetQuestInfo(DuplicateObject(RuntimeNode->QuestInfo, RuntimeNode));
		}
		else
		{
			NewNode->InitNodeInfo(NewNode);
		}

		for (UQuestRuntimePin* Pin : RuntimeNode->InputPins)
		{
			UEdGraphPin* UiPin = NewNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Input, Pin->PinName);
			UiPin->PinId = Pin->PinId;
			IdToPinMap.Add(Pin->PinId, UiPin);
		}

		for (UQuestRuntimePin* Pin : RuntimeNode->OutputPins)
		{
			UEdGraphPin* UiPin = NewNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, Pin->PinName);
			UiPin->PinId = Pin->PinId;
			
			if (Pin->Connection != nullptr) {
				Connections.Add(std::make_pair(Pin->PinId, Pin->Connection->PinId));
			}

			IdToPinMap.Add(Pin->PinId, UiPin);
			
		}

		WorkingGraph->AddNode(NewNode, true, true);
		
	}

	for (std::pair<FGuid, FGuid> Connection : Connections) {
		UEdGraphPin* FromPin = IdToPinMap[Connection.first];
		UEdGraphPin* ToPin = IdToPinMap[Connection.second];
		FromPin->LinkedTo.Add(ToPin);
		ToPin->LinkedTo.Add(FromPin);
	}
	
	
}

UQuestGraphNodeBase* FChainQuestAssetEditorApp::GetSelectedNode(const FGraphPanelSelectionSet& Selection)
{
	for (UObject* Object : Selection)
	{
		UQuestGraphNodeBase* Node = Cast<UQuestGraphNodeBase>(Object);
		if (Node != nullptr)
		{
			return Node;
		}
	}
	return nullptr;
}

void FChainQuestAssetEditorApp::CreateGraphEditorCommands()
{
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CanCopyNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CanCutNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CanPasteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CanDeleteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FChainQuestAssetEditorApp::CanDuplicateNodes));
}

void FChainQuestAssetEditorApp::CopySelectedNodes()
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

bool FChainQuestAssetEditorApp::CanCopyNodes() const
{
	return WorkingGraphUI && WorkingGraphUI->GetSelectedNodes().Num() > 0;
}

void FChainQuestAssetEditorApp::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

bool FChainQuestAssetEditorApp::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FChainQuestAssetEditorApp::PasteNodes()
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

bool FChainQuestAssetEditorApp::CanPasteNodes() const
{
	if (!WorkingGraph) return false;
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(WorkingGraph, ClipboardContent);
}

void FChainQuestAssetEditorApp::DeleteSelectedNodes()
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

bool FChainQuestAssetEditorApp::CanDeleteNodes() const
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

void FChainQuestAssetEditorApp::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FChainQuestAssetEditorApp::CanDuplicateNodes() const
{
	return CanCopyNodes();
}
