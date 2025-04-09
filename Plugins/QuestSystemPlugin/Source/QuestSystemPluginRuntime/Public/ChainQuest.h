
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


class FChainQuestHandler;
class UCondition;
class UChainQuestGraph;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:

	FChainQuestHandler GetHandler() const;

	UChainQuest();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCondition> StartCondition;
	
	UPROPERTY(BlueprintReadWrite)
	UChainQuestGraph* ChainQuestGraph = nullptr;

	
};
																																							