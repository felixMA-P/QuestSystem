#include "ChainQuestEditorApp.h"
#include "ChainQuest.h"
#include "ChainQuestAssetAppMode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Schemas/ChainQuestGraphSchema.h"

void FChainQuestEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
	
}

void FChainQuestEditorApp::InitEditor(const EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost>& InitToolKitHost,
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
	AddApplicationMode(TEXT("ChainQuestAppMode"), MakeShareable(new FChainQuestAssetAppMode(SharedThis(this))));
	SetCurrentMode(TEXT("ChainQuestAppMode"));
}
