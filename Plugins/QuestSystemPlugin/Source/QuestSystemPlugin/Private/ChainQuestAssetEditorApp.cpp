#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"
#include "ChainQuestAssetAppMode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Schemas/ChainQuestGraphSchema.h"
#include "ChainQuestGraph.h"
#include "Schemas/QuestGraphNode.h"
#include "Schemas/QuestGraphNodeBase.h"
#include "Schemas/QuestKnotNode.h"
#include "QuestInfo.h"
#include "Schemas/QuestStartGraphNode.h"
#include "Schemas/QuestEndGraphNode.h"
#include "QuestNodeType.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"
#include "ScopedTransaction.h"

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

	GEditor->RegisterForUndo(this);

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
	if (WorkingGraph)
	{
		WorkingGraph->RemoveOnGraphChangedHandler(GraphChangeListenerHandle);
	}
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
	SyncAllInputPinColors();
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

namespace
{
	struct FPendingQuestConnection
	{
		FGuid FromPinId;
		FGuid ToPinId;
		TArray<FVector2D> ReroutePoints;
	};

	// Walks through any chain of non-compiler-relevant nodes (reroute/knot nodes) linked to Pin,
	// collecting their positions, and returns the pin on the first real (compiler-relevant) node found.
	UEdGraphPin* ResolveRealQuestPin(UEdGraphPin* Pin, TArray<FVector2D>& OutReroutePoints, TSet<UEdGraphNode*>& Visited)
	{
		if (!Pin->HasAnyConnections()) return nullptr;

		UEdGraphPin* Next = Pin->LinkedTo[0];
		UEdGraphNode* Owner = Next->GetOwningNode();

		if (Owner && !Owner->IsCompilerRelevant())
		{
			if (Visited.Contains(Owner)) return nullptr;
			Visited.Add(Owner);
			OutReroutePoints.Add(FVector2D(Owner->NodePosX, Owner->NodePosY));

			UEdGraphPin* PassThrough = Owner->GetPassThroughPin(Next);
			return PassThrough ? ResolveRealQuestPin(PassThrough, OutReroutePoints, Visited) : nullptr;
		}

		return Next;
	}
}

void FChainQuestAssetEditorApp::UpdateWorkingAssetFromGraph()
{
	if (WorkingAsset == nullptr || WorkingGraph == nullptr) return;

	UChainQuestGraph* RuntimeGraph = NewObject<UChainQuestGraph>(WorkingAsset);
	WorkingAsset->ChainQuestGraph = RuntimeGraph;

	TArray<FPendingQuestConnection> Connections;
	TMap<FGuid, UQuestRuntimePin*> IdToPinMap;

	for (UEdGraphNode* UiNode : WorkingGraph->Nodes)
	{
		UQuestGraphNodeBase* UIQuestGraphNode = Cast<UQuestGraphNodeBase>(UiNode);
		if (!UIQuestGraphNode)
		{
			UE_LOG(LogTemp, Warning, TEXT("ChainQuest editor: skipping non-quest node '%s' while saving"), *UiNode->GetName());
			continue;
		}

		if (UIQuestGraphNode->GetQuestNodeType() == EQuestNodeType::KnotNode)
		{
			continue;
		}

		UQuestRuntimeNode* RuntimeNode = NewObject<UQuestRuntimeNode>(RuntimeGraph);
		RuntimeNode->Position = FVector2D(UiNode->NodePosX, UiNode->NodePosY);

		for (UEdGraphPin* UiPin : UiNode->Pins)
		{
			UQuestRuntimePin* RuntimePin = NewObject<UQuestRuntimePin>(RuntimeNode);
			RuntimePin->PinName = UiPin->PinName;
			RuntimePin->PinId = UiPin->PinId;
			RuntimePin->Parent = RuntimeNode;

			if (UiPin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				TArray<FVector2D> ReroutePoints;
				TSet<UEdGraphNode*> Visited;
				if (UEdGraphPin* RealDest = ResolveRealQuestPin(UiPin, ReroutePoints, Visited))
				{
					Connections.Add({ UiPin->PinId, RealDest->PinId, MoveTemp(ReroutePoints) });
				}
			}

			IdToPinMap.Add(UiPin->PinId, RuntimePin);

			if (UiPin->Direction == EEdGraphPinDirection::EGPD_Input) {
				RuntimeNode->InputPins.Add(RuntimePin);
			} else {
				RuntimeNode->OutputPins.Add(RuntimePin);
			}
		}

		RuntimeNode->QuestNodeType = UIQuestGraphNode->GetQuestNodeType();
		RuntimeNode->QuestInfo = DuplicateObject(UIQuestGraphNode->GetQuestInfoBase(), RuntimeNode);

		RuntimeGraph->Nodes.Add(RuntimeNode);
	}

	for (const FPendingQuestConnection& Connection : Connections)
	{
		UQuestRuntimePin** pin1 = IdToPinMap.Find(Connection.FromPinId);
		UQuestRuntimePin** pin2 = IdToPinMap.Find(Connection.ToPinId);
		if (!pin1 || !pin2) continue;

		(*pin1)->Connection = *pin2;
		(*pin1)->ReroutePoints = Connection.ReroutePoints;
	}

}

void FChainQuestAssetEditorApp::UpdateEditorGraphFromWorkingAsset()
{
	if (WorkingAsset == nullptr) return;

	if (WorkingAsset->ChainQuestGraph == nullptr)
	{
		WorkingGraph->GetSchema()->CreateDefaultNodesForGraph(*WorkingGraph);
		SyncAllInputPinColors();
		return;
	}

	TArray<FPendingQuestConnection> Connections;
	TMap<FGuid, UEdGraphPin*> IdToPinMap;

	for (UQuestRuntimeNode* RuntimeNode : WorkingAsset->ChainQuestGraph->Nodes)
	{

		UQuestGraphNodeBase* NewNode = nullptr;
		if (RuntimeNode->QuestNodeType == EQuestNodeType::QuestNode)
			NewNode = NewObject<UQuestGraphNode>(WorkingGraph, NAME_None, RF_Transactional);
		else if (RuntimeNode->QuestNodeType == EQuestNodeType::StartNode)
			NewNode = NewObject<UQuestStartGraphNode>(WorkingGraph, NAME_None, RF_Transactional);
		else if (RuntimeNode->QuestNodeType == EQuestNodeType::EndNode)
			NewNode = NewObject<UQuestEndGraphNode>(WorkingGraph, NAME_None, RF_Transactional);

		if (!NewNode)
		{
			UE_LOG(LogTemp, Warning, TEXT("ChainQuest editor: skipping node with unhandled QuestNodeType while loading '%s'"), *WorkingAsset->GetName());
			continue;
		}

		NewNode->CreateNewGuid();
		NewNode->NodePosX = RuntimeNode->Position.X;
		NewNode->NodePosY = RuntimeNode->Position.Y;

		if (RuntimeNode->QuestInfo != nullptr)
		{
			UQuestInfoBase* DuplicatedInfo = DuplicateObject(RuntimeNode->QuestInfo, RuntimeNode);
			DuplicatedInfo->SetFlags(RF_Transactional);
			NewNode->SetQuestInfo(DuplicatedInfo);
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

		UQuestInfo* NodeQuestInfo = Cast<UQuestInfo>(NewNode->GetQuestInfoBase());
		int32 OutputPinIndex = 0;
		for (UQuestRuntimePin* Pin : RuntimeNode->OutputPins)
		{
			UEdGraphPin* UiPin = NewNode->CreateCustomPin(EEdGraphPinDirection::EGPD_Output, Pin->PinName);
			UiPin->PinId = Pin->PinId;

			// Regular quest-node outputs use their saved color; the Start node has no backing OutPuts entry, so its
			// single output always falls back to the default palette's first color.
			if (NodeQuestInfo && NodeQuestInfo->OutPuts.IsValidIndex(OutputPinIndex))
			{
				UQuestGraphNodeBase::TagPinWithBranchColor(UiPin, NodeQuestInfo->OutPuts[OutputPinIndex].Color);
			}
			else
			{
				UQuestGraphNodeBase::TagPinWithBranchColor(UiPin, UQuestGraphNodeBase::GetBranchColor(OutputPinIndex));
			}
			OutputPinIndex++;

			if (Pin->Connection != nullptr) {
				Connections.Add({ Pin->PinId, Pin->Connection->PinId, Pin->ReroutePoints });
			}

			IdToPinMap.Add(Pin->PinId, UiPin);

		}

		WorkingGraph->AddNode(NewNode, true, true);
		
	}

	for (const FPendingQuestConnection& Connection : Connections) {
		UEdGraphPin** FromPin = IdToPinMap.Find(Connection.FromPinId);
		UEdGraphPin** ToPin = IdToPinMap.Find(Connection.ToPinId);
		if (!FromPin || !ToPin) continue;

		UEdGraphPin* PrevPin = *FromPin;
		for (const FVector2D& Point : Connection.ReroutePoints)
		{
			UQuestKnotNode* Knot = NewObject<UQuestKnotNode>(WorkingGraph, UQuestKnotNode::StaticClass(), NAME_None, RF_Transactional);
			Knot->CreateNewGuid();
			Knot->NodePosX = Point.X;
			Knot->NodePosY = Point.Y;
			Knot->CreateKnotInputPin();
			Knot->CreateKnotOutputPin();
			WorkingGraph->AddNode(Knot, true, true);

			PrevPin->LinkedTo.Add(Knot->GetInputPin());
			Knot->GetInputPin()->LinkedTo.Add(PrevPin);
			PrevPin = Knot->GetOutputPin();
		}

		PrevPin->LinkedTo.Add(*ToPin);
		(*ToPin)->LinkedTo.Add(PrevPin);
	}

	SyncAllInputPinColors();
}

void FChainQuestAssetEditorApp::SyncAllInputPinColors()
{
	if (!WorkingGraph) return;

	for (UEdGraphNode* Node : WorkingGraph->Nodes)
	{
		// Knot nodes are pass-throughs, not part of OutPuts, and must keep their fixed "QuestKnotPin" subcategory.
		if (UQuestGraphNodeBase* QuestNode = Cast<UQuestGraphNodeBase>(Node); QuestNode && QuestNode->GetQuestNodeType() != EQuestNodeType::KnotNode)
		{
			QuestNode->SyncInputPinColors();
		}
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

	const FScopedTransaction Transaction(FText::FromString("Paste Quest Nodes"));

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

	const FScopedTransaction Transaction(FText::FromString("Delete Quest Nodes"));

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

bool FChainQuestAssetEditorApp::MatchesContext(const FTransactionContext& InContext,
	const TArray<TPair<UObject*, FTransactionObjectEvent>>& TransactionObjectContexts) const
{
	return FEditorUndoClient::MatchesContext(InContext, TransactionObjectContexts);
}

void FChainQuestAssetEditorApp::PostUndo(bool bSuccess)
{
	WorkingGraph->Modify();
	WorkingGraph->NotifyGraphChanged();
	FEditorUndoClient::PostUndo(bSuccess);
}

void FChainQuestAssetEditorApp::PostRedo(bool bSuccess)
{
	WorkingGraph->Modify();
	WorkingGraph->NotifyGraphChanged();
	FEditorUndoClient::PostRedo(bSuccess);
}
