#include "DialogFunctionHelper.h"

UDialogWorldSubsystem* UDialogFunctionHelper::GetDialogSystem(UWorld* WorldContext)
{
	return WorldContext->GetSubsystem<UDialogWorldSubsystem>();
}
