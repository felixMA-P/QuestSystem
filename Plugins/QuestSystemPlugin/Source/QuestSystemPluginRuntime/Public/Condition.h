
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Condition.generated.h"


class UQuestWorldSubsystem;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API ACondition : public AActor
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition(const UQuestWorldSubsystem* QuestWorldSubsystem);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool CheckConditionEvent(const UQuestWorldSubsystem* QuestWorldSubsystem);
};
