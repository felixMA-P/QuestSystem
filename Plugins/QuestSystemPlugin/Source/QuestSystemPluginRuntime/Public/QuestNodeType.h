// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "QuestNodeType.generated.h"

UENUM()
enum class EQuestNodeType
{
	StartNode,
	QuestNode,
	EndNode,
	KnotNode,
	Unknown
};