#pragma once

#include "CoreMinimal.h"
#include "DialogInfoBase.h"
#include "DialogNodeType.h"
#include "UObject/Object.h"
#include "DialogGraph.generated.h"

class UDialogRuntimeNode;

UCLASS()
class DIALOGPLUGINRUNTIME_API UDialogRuntimePin : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY() FName PinName;
	UPROPERTY() FGuid PinId;
	UPROPERTY() UDialogRuntimePin* Connection = nullptr;
	UPROPERTY() UDialogRuntimeNode* Parent = nullptr;

	// Positions of any reroute (knot) nodes bent into the wire between this pin and Connection, editor-visual only.
	UPROPERTY() TArray<FVector2D> ReroutePoints;
};

UCLASS(BlueprintType)
class DIALOGPLUGINRUNTIME_API UDialogRuntimeNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY() TArray<UDialogRuntimePin*> InputPins;
	UPROPERTY() TArray<UDialogRuntimePin*> OutputPins;
	UPROPERTY() FVector2D Position;

	UPROPERTY(BlueprintReadOnly, Category = "RuntimeNode")
	UDialogInfoBase* DialogInfo;

	UPROPERTY() EDialogNodeType DialogNodeType = EDialogNodeType::DialogNode;
};

UCLASS(BlueprintType)
class DIALOGPLUGINRUNTIME_API UDialogGraph : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Dialog System")
	TArray<UDialogRuntimeNode*> Nodes;
};
