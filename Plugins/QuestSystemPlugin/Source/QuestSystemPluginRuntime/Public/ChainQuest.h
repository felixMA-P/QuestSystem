
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


class UChainQuestGraph;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FText Title;

	UPROPERTY()
	UChainQuestGraph* ChainQuestGraph = nullptr;
};
