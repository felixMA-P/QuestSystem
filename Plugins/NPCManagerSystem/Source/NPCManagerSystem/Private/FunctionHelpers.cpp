// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#include "FunctionHelpers.h"

#include "Engine/World.h"

UNpcManagerSubsystem* UFunctionHelpers::GetNpcManager(UWorld* WorldContext)
{
	return WorldContext ? WorldContext->GetSubsystem<UNpcManagerSubsystem>() : nullptr;
}
