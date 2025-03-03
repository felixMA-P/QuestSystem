#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestNodeType.h"
#include "QuestInfoBase.h"
#include "QuestGraphNodeBase.generated.h"

UCLASS()
class UQuestGraphNodeBase: public UEdGraphNode
{
	GENERATED_BODY()
	
public:

	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) { return nullptr; }
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutPutPins() {}

	virtual void InitNodeInfo(UObject* Output) {};
	virtual void SetQuestInfo(UQuestInfoBase* Info) {}
	virtual UQuestInfoBase* GetQuestInfoBase () const { return nullptr; }

	virtual EQuestNodeType GetQuestNodeType() const { return EQuestNodeType::Unknown; }

	virtual void OnPropertiesChanged() { }

protected:

	
};
