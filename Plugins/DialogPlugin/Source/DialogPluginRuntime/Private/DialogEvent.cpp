#include "DialogEvent.h"

UDialogEvent::UDialogEvent()
{
}

void UDialogEvent::ExecuteEvent(UObject* WorldContextObject)
{
	ExecuteEventBP(WorldContextObject);
}
