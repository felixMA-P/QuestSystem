#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogNodeType.h"
#include "DialogInfoBase.h"
#include "DialogGraphNodeBase.generated.h"

UCLASS()
class UDialogGraphNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) { return nullptr; }
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutputPins() {}

	virtual void InitNodeInfo(UObject* Outer) {}
	virtual void SetDialogInfo(UDialogInfoBase* Info) {}
	virtual UDialogInfoBase* GetDialogInfoBase() const { return nullptr; }

	virtual EDialogNodeType GetDialogNodeType() const { return EDialogNodeType::Unknown; }

	virtual void OnPropertiesChanged() {}
};
