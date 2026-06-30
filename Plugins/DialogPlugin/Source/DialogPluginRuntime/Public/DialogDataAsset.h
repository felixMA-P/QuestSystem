#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DialogDataAsset.generated.h"

UCLASS()
class DIALOGPLUGINRUNTIME_API UDialogDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialog System")
	TMap<FGameplayTag ,UDialog*> Dialogs;
};
