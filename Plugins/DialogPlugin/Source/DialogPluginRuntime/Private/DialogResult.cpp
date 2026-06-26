#include "DialogResult.h"

UDialogResult::UDialogResult()
{
}

void UDialogResult::ExecuteResult(UWorld* World)
{
	ExecuteResultEvent(World);
}
