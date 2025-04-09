// Fill out your copyright notice in the Description page of Project Settings.


#include "Condition.h"
#include "QuestWorldSubsystem.h"

bool UCondition::CheckCondition(UQuestWorldSubsystem* QuestComponent)
{
	return CheckConditionEvent(QuestComponent);
}
