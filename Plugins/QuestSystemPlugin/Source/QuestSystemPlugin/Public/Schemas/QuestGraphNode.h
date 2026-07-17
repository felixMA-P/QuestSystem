// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QuestGraphNodeBase.h"
#include "QuestInfo.h"
#include "EdGraph/EdGraphNode.h"
#include "QuestGraphNode.generated.h"


UCLASS()
class UQuestGraphNode : public UQuestGraphNodeBase
{

	GENERATED_BODY()
	
public:
	UQuestGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Green; }
	virtual bool CanUserDeleteNode() const override {return true;}
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

public: //Our interface
	
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;
	void SyncPinsWithOutputs();

	virtual EQuestNodeType GetQuestNodeType() const override { return EQuestNodeType::QuestNode; }
	
	virtual UEdGraphPin* CreateDefaultInputPin() override;
	virtual void CreateDefaultOutPutPins() override;

	virtual void InitNodeInfo(UObject* Output) override { QuestInfo = NewObject<UQuestInfo>(Output, NAME_None, RF_Transactional); }
	virtual void SetQuestInfo(UQuestInfoBase* Info) override { QuestInfo = Cast<UQuestInfo>(Info); }
	virtual UQuestInfoBase* GetQuestInfoBase () const override { return QuestInfo; }
	virtual UQuestInfo* GetQuestInfo () const { return QuestInfo; }

	virtual void OnPropertiesChanged() override { SyncPinsWithOutputs(); }

	// Opens the native color picker for OutPuts[OutputIndex] and applies the chosen color live.
	void OpenPinColorPicker(int32 OutputIndex);

private:

	void HandlePinColorPicked(FLinearColor NewColor, int32 OutputIndex);

	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;
	
	UPROPERTY()
	UQuestInfo* QuestInfo = nullptr;
	
};
