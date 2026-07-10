// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestCondition.h"
#include "QuestWorldSubsystem.h"

bool UQuestCondition::CheckCondition(UWorld* World)
{
	return CheckConditionEvent(World);
}
