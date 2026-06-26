#pragma once

#include "CoreMinimal.h"
#include "DialogCondition.h"
#include "DialogInfoBase.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DialogLineInfo.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogLineInfo : public UDialogInfoBase
{
	GENERATED_BODY()

public:
	UDialogLineInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	FGameplayTag Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	FText DialogText;

	// Key = condition that must pass; Value = player-facing choice text shown in UI
	UPROPERTY(EditAnywhere, NoClear, Category = "DialogLine")
	TMap<TSubclassOf<UDialogCondition>, FText> Outputs;
};
