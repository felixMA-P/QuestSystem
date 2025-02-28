#include "ChainQuestAssetEditorApp.h"
#include "ChainQuest.h"
#include "ChainQuestAssetAppMode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Schemas/ChainQuestGraphSchema.h"
#include "ChainQuestGraph.h"
#include "Schemas/QuestGraphNode.h"
#include "QuestInfo.h"
#include "Schemas/QuestStartGraphNode.h"
#include  "QuestNodeType.h"

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

void FChainQuestAssetEditorApp::OnGraphChanged(const FEdGraphEditAction& EditAction)
{
	UpdateWorkingAssetFromGraph();
}

void FChainQuestAssetEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if(WorkingGraphUI != nullptr)
	{
		UQuestGraphNode* QuestGraphNode = GetSelectedNode(WorkingGraphUI->GetSelectedNodes());
		if(QuestGraphNode)
		{
			QuestGraphNode->SyncPinsWithOutputs();
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
	UQuestGraphNode* Node = GetSelectedNode(Selection);
	if (Node)
	{
		DetailsView->SetObject(Node->GetQuestInfo());
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

		if (UiNode->IsA(UQuestGraphNode::StaticClass()))
		{
			UQuestGraphNode* UIQuestGraphNode = Cast<UQuestGraphNode>(UiNode);
			RuntimeNode->QuestNodeType = EQuestNodeType::QuestNode;
			RuntimeNode->QuestInfo = UIQuestGraphNode->GetQuestInfo();
		}
		else if (UiNode->IsA(UQuestStartGraphNode::StaticClass()))
		{
			RuntimeNode->QuestNodeType = EQuestNodeType::StartNode;
		}
		
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
		else if (RuntimeNode->QuestNodeType == EQuestNodeType::QuestNode)
			NewNode = NewObject<UQuestStartGraphNode>(WorkingGraph);

		check(NewNode);
		
		NewNode->CreateNewGuid();
		NewNode->NodePosX = RuntimeNode->Position.X;
		NewNode->NodePosY = RuntimeNode->Position.Y;

		if (RuntimeNode->QuestInfo != nullptr)
		{
			NewNode->SetQuestInfo(DuplicateObject(RuntimeNode->QuestInfo, RuntimeNode));
		}
		else if (RuntimeNode->QuestNodeType != EQuestNodeType::StartNode)
		{
			NewNode->SetQuestInfo(NewObject<UQuestInfo>(RuntimeNode));
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

UQuestGraphNode* FChainQuestAssetEditorApp::GetSelectedNode(const FGraphPanelSelectionSet& Selection)
{
	for (UObject* Object : Selection)
	{
		UQuestGraphNode* Node = Cast<UQuestGraphNode>(Object);
		if (Node != nullptr)
		{
			return Node;
		}
	}
	return nullptr;
}
