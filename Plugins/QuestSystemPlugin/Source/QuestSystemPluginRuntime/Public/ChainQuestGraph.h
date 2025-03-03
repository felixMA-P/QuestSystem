
#pragma once

#include "CoreMinimal.h"
#include "QuestInfoBase.h"
#include "QuestNodeType.h"
#include "UObject/Object.h"
#include "ChainQuestGraph.generated.h"

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
};

UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UQuestRuntimeNode : public UObject {
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UQuestRuntimePin*> InputPins;

	UPROPERTY()
	TArray<UQuestRuntimePin*> OutputPins;

	UPROPERTY()
	FVector2D Position;

	UPROPERTY()
	UQuestInfoBase* QuestInfo;

	UPROPERTY()
	EQuestNodeType QuestNodeType = EQuestNodeType::QuestNode;
};

UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UChainQuestGraph : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY();
	TArray<UQuestRuntimeNode*> Nodes;
};
