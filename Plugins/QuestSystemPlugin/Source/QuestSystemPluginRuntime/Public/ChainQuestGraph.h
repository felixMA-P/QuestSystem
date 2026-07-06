
#pragma once

#include "CoreMinimal.h"
#include "QuestInfoBase.h"
#include "QuestNodeType.h"
#include "UObject/Object.h"
#include "ChainQuestGraph.generated.h"

class UQuestRuntimeNode;
class UQuestInfo;

UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UQuestRuntimePin : public UObject {
	GENERATED_BODY()

public:
	
	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FGuid PinId;

	UPROPERTY()
	UQuestRuntimePin* Connection = nullptr;

	UPROPERTY()
	UQuestRuntimeNode* Parent = nullptr;

	// Positions of any reroute (knot) nodes bent into the wire between this pin and Connection, editor-visual only.
	UPROPERTY()
	TArray<FVector2D> ReroutePoints;
};

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UQuestRuntimeNode : public UObject {
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UQuestRuntimePin*> InputPins;

	UPROPERTY()
	TArray<UQuestRuntimePin*> OutputPins;

	UPROPERTY()
	FVector2D Position;

	UPROPERTY(BlueprintReadOnly, Category = "RuntimeNode")
	UQuestInfoBase* QuestInfo;

	UPROPERTY()
	EQuestNodeType QuestNodeType = EQuestNodeType::QuestNode;
};

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuestGraph : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Quest System")
	TArray<UQuestRuntimeNode*> Nodes;
};
