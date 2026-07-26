#pragma once

#include "CoreMinimal.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "K2Node_RunNPCAction.generated.h"

class FKismetCompilerContext;
class UEdGraph;
class UEdGraphPin;

/**
 *
 * Blueprint node: queues a UNPCAction on an NPC, exposing the chosen action class's
 * Expose-on-Spawn properties as input pins — the same behaviour as Construct Object
 * from Class or Spawn Actor from Class.
 *
 * Expands to UNpcManagerSubsystem::CreateNPCAction -> property assignments -> QueueAction,
 * so one action Blueprint can be reused with its parameters supplied at the call site.
 *
 * Felix Martin Arroyo
 */
UCLASS()
class UK2Node_RunNPCAction : public UK2Node_ConstructObjectFromClass
{
	GENERATED_BODY()

public:

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	//~ Begin UK2Node_ConstructObjectFromClass Interface
	virtual UClass* GetClassPinBaseClass() const override;
	virtual bool UseWorldContext() const override { return false; }
	virtual bool UseOuter() const override { return false; }
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) const override;
	virtual FText GetBaseNodeTitle() const override;
	virtual FText GetDefaultNodeTitle() const override;
	virtual FText GetNodeTitleFormat() const override;
	//~ End UK2Node_ConstructObjectFromClass Interface

	/** The UNpcManagerSubsystem the action is queued on. */
	UEdGraphPin* GetTargetPin() const;

	/** Tag of the NPC whose queue the action joins. */
	UEdGraphPin* GetNPCTagPin() const;
};
