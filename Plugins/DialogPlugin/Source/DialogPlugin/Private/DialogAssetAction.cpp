#include "DialogAssetAction.h"
#include "Dialog.h"
#include "DialogAssetEditorApp.h"

FDialogAssetAction::FDialogAssetAction(const EAssetTypeCategories::Type InAssetCategory)
{
	AssetCategory = InAssetCategory;
}

FText FDialogAssetAction::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DialogAsset", "Dialog Asset");
}

FColor FDialogAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FDialogAssetAction::GetSupportedClass() const
{
	return UDialog::StaticClass();
}

void FDialogAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UDialog* Dialog = Cast<UDialog>(Object);
		if (Dialog)
		{
			TSharedRef<FDialogAssetEditorApp> Editor(new FDialogAssetEditorApp());
			Editor->InitEditor(Mode, EditWithinLevelEditor, Dialog);
		}
	}
}

uint32 FDialogAssetAction::GetCategories()
{
	return AssetCategory;
}
