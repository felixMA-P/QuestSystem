#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogEvent.generated.h"

// Subclass this in Blueprint or C++ to run logic when the player selects a specific dialog response.
// ExecuteEventBP is the Blueprint-implementable entry point.
UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogEvent : public UObject
{
	GENERATED_BODY()

public:
	UDialogEvent();

	virtual void ExecuteEvent(UWorld* World);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteEventBP(UWorld* World);
};
