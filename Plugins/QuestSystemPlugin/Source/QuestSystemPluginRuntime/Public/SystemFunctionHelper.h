// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestWorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SystemFunctionHelper.generated.h"

/**
 * 
 */
UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API USystemFunctionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	USystemFunctionHelper(){};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CustomSubsystemsAccess")
	static UQuestWorldSubsystem* GetQuestSystem(UWorld* WorldContext)
	{
		return WorldContext->GetSubsystem<UQuestWorldSubsystem>();
	}
	
};

