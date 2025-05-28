// Fill out your copyright notice in the Description page of Project Settings.


#include "Condition.h"
#include "QuestWorldSubsystem.h"

bool ACondition::CheckCondition(const UQuestWorldSubsystem* QuestWorldSubsystem)
{
	return CheckConditionEvent(QuestWorldSubsystem);
}
