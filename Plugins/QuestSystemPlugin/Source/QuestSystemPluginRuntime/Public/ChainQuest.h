
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Condition.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


class FChainQuestHandler;
class UChainQuestGraph;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:

	FChainQuestHandler* GetHandler() const;

	UChainQuest();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChainQuestQuest")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChainQuestQuest")
	bool bHasCalendarDates = false;

	UPROPERTY(EditAnywhere, Category = "ChainQuestQuest")
	TSubclassOf<UCondition> StartCondition;
	
	UPROPERTY(BlueprintReadWrite, Category = "ChainQuestQuest")
	UChainQuestGraph* ChainQuestGraph = nullptr;

	
};
																																							