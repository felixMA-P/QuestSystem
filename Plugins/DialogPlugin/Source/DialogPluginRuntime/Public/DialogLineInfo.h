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

	UPROPERTY(EditAnywhere, Instanced, Category = "DialogOutput")
	UDialogEvent* Event = nullptr;
};

UCLASS(BlueprintType)
class DIALOGPLUGINRUNTIME_API UDialogLineInfo : public UDialogInfoBase
{
	GENERATED_BODY()

public:
	UDialogLineInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	FGameplayTag Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	FText DialogResume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	FText DialogText;

	// Wraps the graph node title preview at this many characters; does not affect DialogText itself. 0 disables wrapping.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine", meta = (ClampMin = "0", UIMin = "0"))
	int32 GraphPreviewWrapLength = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogLine")
	TArray<FDialogOutput> Outputs;
};
