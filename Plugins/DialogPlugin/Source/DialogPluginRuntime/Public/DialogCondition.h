// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogCondition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition(UWorld* World);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool CheckConditionEvent(UWorld* World);
};
