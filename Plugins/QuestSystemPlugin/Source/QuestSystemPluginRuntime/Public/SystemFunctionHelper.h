// Fill out your copyright notice in the Description page of Project Settings.

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

