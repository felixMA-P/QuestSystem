
#pragma once

#include "CoreMinimal.h"
#include "ChainQuest.h"
#include "Engine/DataAsset.h"
#include "DataAssetChainQuests.generated.h"


UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UDataAssetChainQuests : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Quest system")
	TArray<UChainQuest*> ChainQuests;
};
