
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndQuestResult.generated.h"

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMPLUGINRUNTIME_API AEndQuestResult : public AActor
{
	GENERATED_BODY()

public:
	AEndQuestResult();
	
	virtual void ExecuteResult();

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteResultEvent();
	
};
