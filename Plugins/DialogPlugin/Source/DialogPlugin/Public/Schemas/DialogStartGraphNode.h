#pragma once

#include "CoreMinimal.h"
#include "DialogGraphNodeBase.h"
#include "DialogStartGraphNode.generated.h"

UCLASS()
class UDialogStartGraphNode : public UDialogGraphNodeBase
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString("Start"); }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Red); }
	virtual bool CanUserDeleteNode() const override { return false; }

	virtual UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, const FName& Name) override;

	virtual EDialogNodeType GetDialogNodeType() const override { return EDialogNodeType::StartNode; }
};
