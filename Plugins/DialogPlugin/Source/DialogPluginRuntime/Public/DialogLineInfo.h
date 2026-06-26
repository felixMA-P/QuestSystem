#pragma once

#include "CoreMinimal.h"
#include "DialogEvent.h"
#include "DialogInfoBase.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DialogLineInfo.generated.h"

// One player-selectable response on a dialog node.
// ResponseText is the choice shown in UI; Event fires (CDO) when the player picks this response.
USTRUCT(BlueprintType)
struct FDialogOutput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogOutput")
	FText ResponseText;

	UPROPERTY(EditAnywhere, Category = "DialogOutput")
	TSubclassOf<UDialogEvent> Event;
};

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

	UPROPERTY(EditAnywhere, Category = "DialogLine")
	TArray<FDialogOutput> Outputs;
};
