
#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/*
 * The asset itself and what we will see in the editor to create a asset and once we have created it.
 */

class FChainQuestAssetAction : public FAssetTypeActions_Base
{

	
public:
	FChainQuestAssetAction(EAssetTypeCategories::Type InAssetCategory);

	// Returns the type name of the asset
	virtual FText GetName() const override;
	
	virtual FColor GetTypeColor() const override;

	//Returns the data type class that 
	virtual UClass* GetSupportedClass() const override;

	//This function executes when we try to open the asset
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	
	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type AssetCategory;
};
