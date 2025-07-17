// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntitiesSubsystem.h"
#include "NpcManagerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UFunctionHelpers.generated.h"


UCLASS()
class NPCMANAGESYSTEM_API UUFunctionHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UUFunctionHelpers(){}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Subsystems")
	static UNpcManagerSubsystem* GetNpcManager(UWorld* WorldContext)
	{
		return WorldContext->GetSubsystem<UNpcManagerSubsystem>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Subsystems")
	static UEntitiesSubsystem* GetEntitiesManager(UWorld* WorldContext)
	{
		return WorldContext->GetSubsystem<UEntitiesSubsystem>();
	}

};
