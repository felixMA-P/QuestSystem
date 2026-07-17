// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogGraphNodeBase.h"
#include "DialogLineInfo.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogGraphNode.generated.h"

UCLASS()
class UDialogGraphNode : public UDialogGraphNodeBase
{
	GENERATED_BODY()

public:
	UDialogGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Green; }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

public:
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;
	void SyncPinsWithOutputs();

	virtual EDialogNodeType GetDialogNodeType() const override { return EDialogNodeType::DialogNode; }

	virtual UEdGraphPin* CreateDefaultInputPin() override;
	virtual void CreateDefaultOutputPins() override;

	virtual void InitNodeInfo(UObject* Outer) override { DialogInfo = NewObject<UDialogLineInfo>(Outer, NAME_None, RF_Transactional); }
	virtual void SetDialogInfo(UDialogInfoBase* Info) override { DialogInfo = Cast<UDialogLineInfo>(Info); }
	virtual UDialogInfoBase* GetDialogInfoBase() const override { return DialogInfo; }
	virtual UDialogLineInfo* GetDialogLineInfo() const { return DialogInfo; }

	virtual void OnPropertiesChanged() override { SyncPinsWithOutputs(); }

	// Opens the native color picker for Outputs[OutputIndex] and applies the chosen color live.
	void OpenPinColorPicker(int32 OutputIndex);

private:

	void HandlePinColorPicked(FLinearColor NewColor, int32 OutputIndex);

	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;

	UPROPERTY()
	UDialogLineInfo* DialogInfo = nullptr;
};
