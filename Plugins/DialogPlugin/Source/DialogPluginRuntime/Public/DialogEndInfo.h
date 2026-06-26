#pragma once

#include "CoreMinimal.h"
#include "DialogInfoBase.h"
#include "DialogResult.h"
#include "DialogEndInfo.generated.h"

class UDialog;

UCLASS(BlueprintType)
class DIALOGPLUGINRUNTIME_API UDialogEndInfo : public UDialogInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "DialogEnd")
	FString Title;

	UPROPERTY(EditAnywhere, Category = "DialogEnd")
	TSubclassOf<UDialogResult> EndResult;

	UPROPERTY(EditAnywhere, Category = "DialogEnd")
	UDialog* NextDialog = nullptr;
};
