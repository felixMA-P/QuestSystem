#include "DialogFunctionHelper.h"
#include "Engine/World.h"

UDialogWorldSubsystem* UDialogFunctionHelper::GetDialogSystem(UWorld* WorldContext)
{
	return WorldContext->GetSubsystem<UDialogWorldSubsystem>();
}
