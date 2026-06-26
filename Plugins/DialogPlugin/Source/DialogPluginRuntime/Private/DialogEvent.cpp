#include "DialogEvent.h"

UDialogEvent::UDialogEvent()
{
}

void UDialogEvent::ExecuteEvent(UWorld* World)
{
	ExecuteEventBP(World);
}
