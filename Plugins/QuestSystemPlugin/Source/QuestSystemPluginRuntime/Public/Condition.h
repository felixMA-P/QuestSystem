
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Condition.generated.h"


class UQuestComponent;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual void CheckCondition(UQuestComponent* QuestComponent);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void CheckConditionEvent(UQuestComponent* QuestComponent);
};
