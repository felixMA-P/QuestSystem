
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndQuestResult.generated.h"

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API UEndQuestResult : public UObject
{
	GENERATED_BODY()

public:
	UEndQuestResult();
	
	virtual void ExecuteResult(UWorld* World);

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteResultEvent(UWorld* World);
	
};
