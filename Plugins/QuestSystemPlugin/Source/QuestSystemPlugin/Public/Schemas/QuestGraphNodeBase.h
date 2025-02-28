#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestNodeType.h"
#include "QuestGraphNodeBase.generated.h"

UCLASS()
class UQuestGraphNodeBase: public UEdGraphNode
{
	GENERATED_BODY()
	
public:

	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) { return nullptr; }

	void SetQuestInfo(class UQuestInfo* Info) { QuestInfo = Info; }
	UQuestInfo* GetQuestInfo () const { return QuestInfo; }

	virtual EQuestNodeType GetQuestNodeType() const { return EQuestNodeType::Unknown; }

protected:
	UPROPERTY()
	UQuestInfo* QuestInfo = nullptr;

	
};
