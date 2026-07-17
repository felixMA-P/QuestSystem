// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogFunctionHelper.h"
#include "Engine/World.h"

UDialogWorldSubsystem* UDialogFunctionHelper::GetDialogSystem(UWorld* WorldContext)
{
	if (!WorldContext) return nullptr;

	return WorldContext->GetSubsystem<UDialogWorldSubsystem>();
}
