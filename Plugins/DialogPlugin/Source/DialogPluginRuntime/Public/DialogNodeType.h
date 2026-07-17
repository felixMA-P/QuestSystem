// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "DialogNodeType.generated.h"

UENUM()
enum class EDialogNodeType
{
	StartNode,
	DialogNode,
	EndNode,
	KnotNode,
	Unknown
};
