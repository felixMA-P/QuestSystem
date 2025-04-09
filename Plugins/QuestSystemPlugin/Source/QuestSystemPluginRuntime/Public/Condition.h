
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Condition.generated.h"


class UQuestWorldSubsystem;


UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition(UQuestWorldSubsystem* QuestComponent);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool CheckConditionEvent(UQuestWorldSubsystem* QuestComponent);
};
