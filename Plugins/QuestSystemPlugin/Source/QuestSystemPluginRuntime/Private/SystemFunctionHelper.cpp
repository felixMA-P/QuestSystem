// Fill out your copyright notice in the Description page of Project Settings.


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
