// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#include "QuestCondition.h"
#include "QuestWorldSubsystem.h"

bool UQuestCondition::CheckCondition(UWorld* World)
{
	return CheckConditionEvent(World);
}
