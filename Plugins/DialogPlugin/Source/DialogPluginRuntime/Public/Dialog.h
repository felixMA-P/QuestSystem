#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "DialogCondition.h"
#include "UObject/Object.h"
#include "Dialog.generated.h"

class UDialogHandler;
class UDialogGraph;

UCLASS(BlueprintType)
class DIALOGPLUGINRUNTIME_API UDialog : public UObject
{
	GENERATED_BODY()

public:
	UDialogHandler* GetHandler(UObject* Outer) const;

	UDialog();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialog")
	FText Title;

	UPROPERTY(EditAnywhere, Category = "Dialog")
	TSubclassOf<UDialogCondition> StartCondition;

	UPROPERTY(BlueprintReadWrite, Category = "Dialog")
	UDialogGraph* DialogGraph = nullptr;
};
