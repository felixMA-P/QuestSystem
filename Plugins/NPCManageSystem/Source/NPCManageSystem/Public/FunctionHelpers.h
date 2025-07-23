// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntitiesSubsystem.h"
#include "NpcManagerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionHelpers.generated.h"


UCLASS()
class NPCMANAGESYSTEM_API UFunctionHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFunctionHelpers(){}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CustomSubsystemsAccess")
	static UNpcManagerSubsystem* GetNpcManager(UWorld* WorldContext)
	{
		return WorldContext->GetSubsystem<UNpcManagerSubsystem>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CustomSubsystemsAccess")
	static UEntitiesSubsystem* GetEntitiesManager(UWorld* WorldContext)
	{
		return WorldContext->GetSubsystem<UEntitiesSubsystem>();
	}

};
