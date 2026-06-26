#include "DialogCondition.h"

bool UDialogCondition::CheckCondition(UWorld* World)
{
	return CheckConditionEvent(World);
}
