#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "QuestGraphNode.generated.h"

UCLASS()
class UQuestGraphNode : public UEdGraphNode
{

	GENERATED_BODY()
	
public:
	UQuestGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Green; }
	virtual bool CanUserDeleteNode() const override {return true;}
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;


public: //Our interface
	
	UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name);

	void SyncPinsWithOutputs();
	
	void SetQuestInfo(class UQuestInfo* Info) { QuestInfo = Info; }
	UQuestInfo* GetQuestInfo () const { return QuestInfo; }

private:
	
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteOutputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteInputPinDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;
	
	
	UPROPERTY()
	UQuestInfo* QuestInfo = nullptr;
};
