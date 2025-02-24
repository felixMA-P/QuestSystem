
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "QuestInfo.generated.h"


UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UQuestInfo : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere) FText Title = FText::FromString("Quest");
	UPROPERTY(EditAnywhere) FText Description = FText::FromString("Description");

	UPROPERTY(EditAnywhere)
	TArray<FText> OutPuts;
	
private:
	
};
