
#pragma once

#include "CoreMinimal.h"
#include "ChainQuestGraph.h"
#include "Condition.h"
#include "QuestInfoBase.h"
#include "EndQuestInfo.generated.h"

class UChainQuest;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UEndQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Title = TEXT("");

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACondition> EndOutput;

	UPROPERTY(EditAnywhere)
	UChainQuest* NextChainQuest = nullptr;

private:
	
};
