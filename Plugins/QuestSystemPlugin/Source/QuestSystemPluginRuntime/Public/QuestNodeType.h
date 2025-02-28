#pragma once

#include "QuestNodeType.generated.h"

UENUM()
enum class EQuestNodeType
{
	StartNode,
	QuestNode,
	EndNode,
	Unknown
};