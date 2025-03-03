
#pragma once

#include "CoreMinimal.h"
#include "Condition.h"
#include "GameplayTagContainer.h"
#include "QuestInfoBase.h"
#include "UObject/Object.h"
#include "QuestInfo.generated.h"

UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere) FText Title = FText::FromString("Quest");
	UPROPERTY(EditAnywhere) FText Description = FText::FromString("Description");

	UPROPERTY(EditAnywhere)
	FGameplayTag StatusTag;

	UPROPERTY(EditAnywhere, NoClear)
	TMap<TSubclassOf<UCondition>, FText> OutPuts;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCondition> Prueba;
	
private:
	
};
