// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_RunNPCAction.h"

#include "EdGraphSchema_K2.h"
#include "GameplayTagContainer.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"
#include "NPCAction.h"
#include "NpcManagerSubsystem.h"

#define LOCTEXT_NAMESPACE "K2Node_RunNPCAction"

namespace RunNPCActionPins
{
	static const FName Target(TEXT("Target"));
	static const FName NPCTag(TEXT("NPCTag"));
}

void UK2Node_RunNPCAction::AllocateDefaultPins()
{
	// Creates exec/then, the Class pin and the Result pin.
	Super::AllocateDefaultPins();

	UEdGraphPin* TargetPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
		UNpcManagerSubsystem::StaticClass(), RunNPCActionPins::Target);
	TargetPin->PinFriendlyName = LOCTEXT("TargetPinName", "Target");

	UEdGraphPin* TagPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct,
		FGameplayTag::StaticStruct(), RunNPCActionPins::NPCTag);
	TagPin->PinFriendlyName = LOCTEXT("NPCTagPinName", "NPC Tag");
}

UClass* UK2Node_RunNPCAction::GetClassPinBaseClass() const
{
	return UNPCAction::StaticClass();
}

bool UK2Node_RunNPCAction::IsSpawnVarPin(UEdGraphPin* Pin) const
{
	if (Pin->ParentPin)
	{
		return IsSpawnVarPin(Pin->ParentPin);
	}

	// Our two pins are node inputs, not properties to assign on the new action.
	return Super::IsSpawnVarPin(Pin)
		&& Pin->PinName != RunNPCActionPins::Target
		&& Pin->PinName != RunNPCActionPins::NPCTag;
}

FText UK2Node_RunNPCAction::GetBaseNodeTitle() const
{
	return LOCTEXT("RunNPCAction_BaseTitle", "Run Action On NPC");
}

FText UK2Node_RunNPCAction::GetDefaultNodeTitle() const
{
	return LOCTEXT("RunNPCAction_TitleNone", "Run Action On NPC");
}

FText UK2Node_RunNPCAction::GetNodeTitleFormat() const
{
	return LOCTEXT("RunNPCAction_Title", "Run {ClassName} On NPC");
}

FText UK2Node_RunNPCAction::GetTooltipText() const
{
	return LOCTEXT("RunNPCAction_Tooltip",
		"Creates an NPC action of the chosen class, applies its Expose on Spawn values, and queues it "
		"on the NPC registered under the given tag.\n\nThe action starts immediately if that NPC is idle, "
		"otherwise it waits its turn.");
}

FText UK2Node_RunNPCAction::GetMenuCategory() const
{
	return LOCTEXT("RunNPCAction_Category", "NPC Manager System|Actions");
}

UEdGraphPin* UK2Node_RunNPCAction::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(RunNPCActionPins::Target);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);

	return Pin;
}

UEdGraphPin* UK2Node_RunNPCAction::GetNPCTagPin() const
{
	UEdGraphPin* Pin = FindPin(RunNPCActionPins::NPCTag);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);

	return Pin;
}

void UK2Node_RunNPCAction::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName CreateFunctionName = GET_FUNCTION_NAME_CHECKED(UNpcManagerSubsystem, CreateNPCAction);
	static const FName QueueFunctionName = GET_FUNCTION_NAME_CHECKED(UNpcManagerSubsystem, QueueAction);

	// Read before any pin links are moved below.
	UClass* ClassToSpawn = GetClassToSpawn();

	UEdGraphPin* TargetPin = GetTargetPin();
	bool bSucceeded = true;

	// --- CreateNPCAction ---------------------------------------------------------------

	UK2Node_CallFunction* CallCreateNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(CreateFunctionName, UNpcManagerSubsystem::StaticClass());
	CallCreateNode->AllocateDefaultPins();

	{
		UEdGraphPin* NodeExec = GetExecPin();
		UEdGraphPin* CallExec = CallCreateNode->GetExecPin();
		bSucceeded &= NodeExec && CallExec
			&& CompilerContext.MovePinLinksToIntermediate(*NodeExec, *CallExec).CanSafeConnect();
	}

	{
		// Copied, not moved: QueueAction needs the same target below.
		UEdGraphPin* CallSelf = CallCreateNode->FindPin(UEdGraphSchema_K2::PN_Self);
		bSucceeded &= TargetPin && CallSelf
			&& CompilerContext.CopyPinLinksToIntermediate(*TargetPin, *CallSelf).CanSafeConnect();
	}

	UEdGraphPin* CallClassPin = nullptr;
	{
		UEdGraphPin* NodeClassPin = GetClassPin();
		CallClassPin = CallCreateNode->FindPin(TEXT("ActionClass"));

		if (NodeClassPin && CallClassPin)
		{
			CallClassPin->DefaultObject = NodeClassPin->DefaultObject;
			bSucceeded &= CompilerContext.MovePinLinksToIntermediate(*NodeClassPin, *CallClassPin).CanSafeConnect();
		}
		else
		{
			bSucceeded = false;
		}
	}

	UEdGraphPin* CallResultPin = nullptr;
	{
		UEdGraphPin* NodeResultPin = GetResultPin();
		CallResultPin = CallCreateNode->GetReturnValuePin();

		// Narrow the intermediate return type to the concrete class so downstream
		// connections made against the node's Result pin stay valid.
		if (NodeResultPin && CallResultPin)
		{
			CallResultPin->PinType = NodeResultPin->PinType;
		}

		bSucceeded &= NodeResultPin && CallResultPin
			&& CompilerContext.MovePinLinksToIntermediate(*NodeResultPin, *CallResultPin).CanSafeConnect();
	}

	// --- Expose-on-Spawn assignments --------------------------------------------------

	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(
		CompilerContext, SourceGraph, CallCreateNode, this, CallResultPin, ClassToSpawn, CallClassPin);

	// --- QueueAction ------------------------------------------------------------------

	UK2Node_CallFunction* CallQueueNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallQueueNode->FunctionReference.SetExternalMember(QueueFunctionName, UNpcManagerSubsystem::StaticClass());
	CallQueueNode->AllocateDefaultPins();

	{
		// Runs after the assignment nodes, so the action is fully configured before it starts.
		UEdGraphPin* QueueExec = CallQueueNode->GetExecPin();
		bSucceeded &= LastThen && QueueExec
			&& CompilerContext.GetSchema()->TryCreateConnection(LastThen, QueueExec);
	}

	{
		UEdGraphPin* QueueSelf = CallQueueNode->FindPin(UEdGraphSchema_K2::PN_Self);
		bSucceeded &= TargetPin && QueueSelf
			&& CompilerContext.MovePinLinksToIntermediate(*TargetPin, *QueueSelf).CanSafeConnect();
	}

	{
		UEdGraphPin* NodeTagPin = GetNPCTagPin();
		UEdGraphPin* QueueTagPin = CallQueueNode->FindPin(RunNPCActionPins::NPCTag);

		if (NodeTagPin && QueueTagPin)
		{
			bSucceeded &= CompilerContext.MovePinLinksToIntermediate(*NodeTagPin, *QueueTagPin).CanSafeConnect();

			// A tag typed straight into the pin has no links to move, so carry the literal across.
			if (NodeTagPin->LinkedTo.Num() == 0)
			{
				QueueTagPin->DefaultValue = NodeTagPin->DefaultValue;
			}
		}
		else
		{
			bSucceeded = false;
		}
	}

	{
		UEdGraphPin* QueueActionPin = CallQueueNode->FindPin(TEXT("Action"));
		bSucceeded &= CallResultPin && QueueActionPin
			&& CompilerContext.GetSchema()->TryCreateConnection(CallResultPin, QueueActionPin);
	}

	{
		UEdGraphPin* NodeThen = GetThenPin();
		UEdGraphPin* QueueThen = CallQueueNode->GetThenPin();
		bSucceeded &= NodeThen && QueueThen
			&& CompilerContext.MovePinLinksToIntermediate(*NodeThen, *QueueThen).CanSafeConnect();
	}

	BreakAllNodeLinks();

	if (!bSucceeded)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("RunNPCAction_ExpandError", "ICE: Run Action On NPC expansion failed @@").ToString(), this);
	}
}

#undef LOCTEXT_NAMESPACE
