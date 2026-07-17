// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "QuestCondition.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


class FChainQuestHandler;
class UChainQuestGraph;

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:

	FChainQuestHandler* GetHandler() const;

	UChainQuest();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChainQuestQuest")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChainQuestQuest")
	bool bHasCalendarDates = false;

	// When true, this chain's StartCondition and every node output's condition are authored/evaluated as gameplay tags (StartConditionTag / FQuestOutput::ConditionTag) instead of Blueprint UQuestCondition classes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChainQuestQuest")
	bool bUseSimpleConditions = false;

	UPROPERTY(EditAnywhere, Category = "ChainQuestQuest")
	TSubclassOf<UQuestCondition> StartCondition;

	// Used instead of StartCondition when bUseSimpleConditions is enabled — satisfied when this tag is present (exact match) in UQuestWorldSubsystem::QuestGameplayTagsContainer. Unset = always starts.
	UPROPERTY(EditAnywhere, Category = "ChainQuestQuest")
	FGameplayTag StartConditionTag;

	UPROPERTY(BlueprintReadWrite, Category = "ChainQuestQuest")
	UChainQuestGraph* ChainQuestGraph = nullptr;

	
};
																																							