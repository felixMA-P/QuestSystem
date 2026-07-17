// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#include "SystemFunctionHelper.h"
#include "QuestWorldSubsystem.h"
#include "Engine/World.h"

USystemFunctionHelper::USystemFunctionHelper()
{
}

UQuestWorldSubsystem* USystemFunctionHelper::GetQuestSystem(UWorld* WorldContext)
{
	if (!WorldContext) return nullptr;
	return WorldContext->GetSubsystem<UQuestWorldSubsystem>();
}
