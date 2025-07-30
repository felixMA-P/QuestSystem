
#pragma once

#include "CoreMinimal.h"
#include "ChainQuestGraph.h"
#include "Condition.h"
#include "QuestInfoBase.h"
#include "EndQuestInfo.generated.h"

class UEndQuestResult;
class UChainQuest;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UEndQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "EndQuest")
	FString Title = TEXT("");

	UPROPERTY(EditAnywhere, Category = "EndQuest")
	TSubclassOf<UEndQuestResult> EndResult;

	UPROPERTY(EditAnywhere, Category = "EndQuest")
	UChainQuest* NextChainQuest = nullptr;

private:
	
};
