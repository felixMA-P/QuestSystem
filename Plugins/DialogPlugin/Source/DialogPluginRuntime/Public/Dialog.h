#pragma once

#include "CoreMinimal.h"
#include "DialogCondition.h"
#include "UObject/Object.h"
#include "Dialog.generated.h"

class FDialogHandler;
class UDialogGraph;

UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialog : public UObject
{
	GENERATED_BODY()

public:
	FDialogHandler* GetHandler() const;

	UDialog();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialog")
	FText Title;

	UPROPERTY(EditAnywhere, Category = "Dialog")
	TSubclassOf<UDialogCondition> StartCondition;

	UPROPERTY(BlueprintReadWrite, Category = "Dialog")
	UDialogGraph* DialogGraph = nullptr;
};
