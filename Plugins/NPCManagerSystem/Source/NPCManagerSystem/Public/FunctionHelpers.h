// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NpcManagerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionHelpers.generated.h"


UCLASS()
class NPCMANAGERSYSTEM_API UFunctionHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Null if WorldContext is null.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CustomSubsystemsAccess")
	static UNpcManagerSubsystem* GetNpcManager(UWorld* WorldContext);

};
