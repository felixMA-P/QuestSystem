// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "ChainQuestGraph.h"
#include "QuestCondition.h"
#include "QuestInfoBase.h"
#include "EndQuestInfo.generated.h"

class UQuestEvent;
class UChainQuest;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UEndQuestInfo : public UQuestInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "EndQuest")
	FText Title = FText::GetEmpty();

	UPROPERTY(EditAnywhere, Category = "EndQuest")
	TSubclassOf<UQuestEvent> EndEvent;

	UPROPERTY(EditAnywhere, Category = "EndQuest")
	UChainQuest* NextChainQuest = nullptr;

private:
	
};
