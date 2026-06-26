#pragma once

#include "DialogNodeType.generated.h"

UENUM()
enum class EDialogNodeType
{
	StartNode,
	DialogNode,
	EndNode,
	Unknown
};
