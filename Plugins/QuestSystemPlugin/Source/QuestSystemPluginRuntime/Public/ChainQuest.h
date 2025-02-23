
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChainQuest.generated.h"


UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ExampleData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SomeNumber = 0;
};
