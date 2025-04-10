
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

	UPROPERTY(BlueprintReadOnly)
	UQuestInfoBase* QuestInfo;

	UPROPERTY()
	EQuestNodeType QuestNodeType = EQuestNodeType::QuestNode;
};

UCLASS(BlueprintType)
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuestGraph : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<UQuestRuntimeNode*> Nodes;
};
