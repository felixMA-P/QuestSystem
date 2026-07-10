
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestCondition.generated.h"


class UQuestWorldSubsystem;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition(UWorld* World);
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System|Condition")
	bool CheckConditionEvent(UWorld* World);
};
