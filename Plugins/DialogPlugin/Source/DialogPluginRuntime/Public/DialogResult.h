#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogResult.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogResult : public UObject
{
	GENERATED_BODY()

public:
	UDialogResult();

	virtual void ExecuteResult(UWorld* World);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteResultEvent(UWorld* World);
};
