#pragma once

#include "CoreMinimal.h"
#include "DialogWorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DialogFunctionHelper.generated.h"

UCLASS()
class DIALOGPLUGINRUNTIME_API UDialogFunctionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CustomSubsystemsAccess")
	static UDialogWorldSubsystem* GetDialogSystem(UWorld* WorldContext);
};
