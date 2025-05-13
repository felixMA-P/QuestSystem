
#pragma once

#include "CoreMinimal.h"
#include "Condition.h"
#include "GameplayTagContainer.h"
#include "QuestInfoBase.h"
#include "UObject/Object.h"
#include "QuestInfo.generated.h"

UCLASS(BlueprintType, BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:

	UQuestInfo();

	UPROPERTY(EditAnywhere) FText Title = FText::FromString("Quest");
	
	UPROPERTY(EditAnywhere) FText Description = FText::FromString("Description");

	UPROPERTY(EditAnywhere) int DayToComplete = 0;

	UPROPERTY(EditAnywhere, NoClear)
	TMap<TSubclassOf<ACondition>, FText> OutPuts;
	
private:
	
};
