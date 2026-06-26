#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "Engine/DataAsset.h"
#include "DialogDataAsset.generated.h"

UCLASS()
class DIALOGPLUGINRUNTIME_API UDialogDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dialog System")
	TArray<UDialog*> Dialogs;
};
