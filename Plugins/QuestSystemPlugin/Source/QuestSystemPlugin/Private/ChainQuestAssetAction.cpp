#include "ChainQuestAssetAction.h"
#include "ChainQuest.h"
#include "ChainQuestAssetEditorApp.h"

FChainQuestAssetAction::FChainQuestAssetAction(EAssetTypeCategories::Type InAssetCategory)
{
	AssetCategory = InAssetCategory;
}

FText FChainQuestAssetAction::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ChainQuestAsset", "Chain Quest Asset");
}

FColor FChainQuestAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FChainQuestAssetAction::GetSupportedClass() const
{
	return UChainQuest::StaticClass();
}

void FChainQuestAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	//FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UChainQuest* ChainQuest = Cast<UChainQuest>(Object);
		if (ChainQuest)
		{
			TSharedRef<FChainQuestAssetEditorApp> Editor (new FChainQuestAssetEditorApp());
			Editor->InitEditor(Mode, EditWithinLevelEditor, ChainQuest);
		}
	}
}

uint32 FChainQuestAssetAction::GetCategories()
{
	return AssetCategory;
}
