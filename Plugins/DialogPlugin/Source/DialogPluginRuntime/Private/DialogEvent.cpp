// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogEvent.h"

UDialogEvent::UDialogEvent()
{
}

void UDialogEvent::ExecuteEvent(UObject* WorldContextObject)
{
	ExecuteEventBP(WorldContextObject);
}
