
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestEvent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestEvent : public UObject
{
	GENERATED_BODY()

public:
	UQuestEvent();

	virtual void Execute(UWorld* World);

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System|Event")
	void ExecuteEvent(UWorld* World);

};
