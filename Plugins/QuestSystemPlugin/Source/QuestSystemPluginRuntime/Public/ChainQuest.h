
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


class FChainQuestHandler;
class ACondition;
class UChainQuestGraph;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:

	FChainQuestHandler* GetHandler() const;

	UChainQuest();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHasCalendarDates = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACondition> StartCondition;
	
	UPROPERTY(BlueprintReadWrite)
	UChainQuestGraph* ChainQuestGraph = nullptr;

	
};
																																							