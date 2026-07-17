// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SystemFunctionHelper.generated.h"


class UQuestWorldSubsystem;

/**
 * 
 */
UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API USystemFunctionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	USystemFunctionHelper();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CustomSubsystemsAccess")
	static UQuestWorldSubsystem* GetQuestSystem(UWorld* WorldContext);
	
};

