// Fill out your copyright notice in the Description page of Project Settings.


#include "SystemFunctionHelper.h"
#include "QuestWorldSubsystem.h"
#include "Engine/World.h"

USystemFunctionHelper::USystemFunctionHelper()
{
}

UQuestWorldSubsystem* USystemFunctionHelper::GetQuestSystem(UWorld* WorldContext)
{
	return WorldContext->GetSubsystem<UQuestWorldSubsystem>();
}
