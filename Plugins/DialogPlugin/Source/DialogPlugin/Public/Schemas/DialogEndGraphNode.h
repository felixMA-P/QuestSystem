#pragma once

#include "CoreMinimal.h"
#include "DialogEndInfo.h"
#include "DialogGraphNodeBase.h"
#include "DialogEndGraphNode.generated.h"

UCLASS()
class UDialogEndGraphNode : public UDialogGraphNodeBase
{
	GENERATED_BODY()

public:
	UDialogEndGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;
	virtual UEdGraphPin* CreateDefaultInputPin() override;

	virtual EDialogNodeType GetDialogNodeType() const override { return EDialogNodeType::EndNode; }

	virtual void InitNodeInfo(UObject* Outer) override { DialogInfo = NewObject<UDialogEndInfo>(Outer, NAME_None, RF_Transactional); }
	virtual void SetDialogInfo(UDialogInfoBase* Info) override { DialogInfo = Cast<UDialogEndInfo>(Info); }
	virtual UDialogInfoBase* GetDialogInfoBase() const override { return DialogInfo; }
	virtual UDialogEndInfo* GetDialogEndInfo() const { return DialogInfo; }

	virtual void OnPropertiesChanged() override { }

private:
	UPROPERTY()
	UDialogEndInfo* DialogInfo = nullptr;

	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteInputPinDelegate;
};
