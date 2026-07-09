
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Condition.h"
#include "GameplayTagContainer.h"
#include "QuestEvent.h"
#include "QuestInfoBase.h"
#include "UObject/Object.h"
#include "QuestInfo.generated.h"

// One condition-gated output on a quest node.
// Condition picks the pin (first one whose CheckCondition passes wins); Event (optional) fires (CDO) when that condition is the one that advances the quest.
USTRUCT(BlueprintType)
struct FQuestOutput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, NoClear, Category = "QuestOutput")
	TSubclassOf<UCondition> Condition;

	UPROPERTY(EditAnywhere, Category = "QuestOutput")
	FText Text = FText::FromString("Output");

	UPROPERTY(EditAnywhere, Category = "QuestOutput")
	TSubclassOf<UQuestEvent> Event;
};

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:

	UQuestInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") FText Title = FText::FromString("Quest");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") FText Description = FText::FromString("Description");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") int DayToComplete = 0;

	UPROPERTY(EditAnywhere, Category = "QuestInfo")
	TArray<FQuestOutput> OutPuts;

private:

};
