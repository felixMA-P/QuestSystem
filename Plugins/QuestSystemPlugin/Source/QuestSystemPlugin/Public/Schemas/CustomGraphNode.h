#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "CustomGraphNode.generated.h"

UCLASS()
class UCustomGraphNode : public UEdGraphNode
{

	GENERATED_BODY()
	
public:

	UCustomGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString("Custom Graph"); }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Green; }
	virtual bool CanUserDeleteNode() const override {return true;}
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name);

private:
	
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;
	
	
};
