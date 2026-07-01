
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Condition.h"
#include "GameplayTagContainer.h"
#include "QuestInfoBase.h"
#include "UObject/Object.h"
#include "QuestInfo.generated.h"

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:

	UQuestInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") FText Title = FText::FromString("Quest");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") FText Description = FText::FromString("Description");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo") int DayToComplete = 0;

	UPROPERTY(EditAnywhere, NoClear, Category = "QuestInfo")
	TMap<TSubclassOf<UCondition>, FText> OutPuts;
	
private:
	
};
