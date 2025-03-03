
#pragma once

#include "CoreMinimal.h"
#include "QuestInfoBase.h"
#include "EndQuestInfo.generated.h"

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UEndQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString ActionData = TEXT("");

private:
	
};
