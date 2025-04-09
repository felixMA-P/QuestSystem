#pragma once


#include "EndQuestInfo.h"
#include "QuestGraphNodeBase.h"
#include "EndQuestInfo.h"
#include "QuestEndGraphNode.generated.h"

UCLASS()
class UQuestEndGraphNode: public UQuestGraphNodeBase
{
	GENERATED_BODY()
public:

	UQuestEndGraphNode();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override ;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

public: // UDialogGraphNodeBase interface
	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;

	virtual EQuestNodeType GetQuestNodeType() const override { return EQuestNodeType::EndNode; }

	virtual UEdGraphPin* CreateDefaultInputPin() override;

	virtual void InitNodeInfo(UObject* Output) override { QuestInfo = NewObject<UEndQuestInfo>(Output); }
	virtual void SetQuestInfo(UQuestInfoBase* Info) override { QuestInfo = Cast<UEndQuestInfo>(Info); }
	virtual UQuestInfoBase* GetQuestInfoBase () const override { return QuestInfo; }
	virtual UEndQuestInfo* GetEndQuestInfo () const { return QuestInfo; }

	virtual void OnPropertiesChanged() override {Modify();}

private:
	UPROPERTY()
	UEndQuestInfo* QuestInfo = nullptr;
	
	TDelegate<void(), FDefaultDelegateUserPolicy> DeleteNodeDelegate;
	TDelegate<void(), FDefaultDelegateUserPolicy> AddNewInputPinDelegate;
};
