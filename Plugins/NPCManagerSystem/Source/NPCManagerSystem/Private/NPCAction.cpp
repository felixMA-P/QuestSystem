// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAction.h"


void UNPCAction::BeginAction()
{
	BeginActionEvent();
}

void UNPCAction::AbortAction()
{
	AbortActionEvent();
}

void UNPCAction::Start(AActor* InTarget, const FGameplayTag& InTag)
{
	TargetNPC = InTarget;
	TargetTag = InTag;
	bRunning = true;
	bFinished = false;

	BeginAction();
}

void UNPCAction::FinishAction()
{
	if (!bRunning || bFinished)
	{
		return;
	}

	bFinished = true;
	bRunning = false;

	OnFinished.Broadcast(this);
	OnCompletedInternal.ExecuteIfBound(this);
}

void UNPCAction::Abort()
{
	if (bFinished)
	{
		return;
	}

	bFinished = true;

	// Queued-but-never-started actions have nothing to tear down.
	const bool bWasRunning = bRunning;
	bRunning = false;

	if (bWasRunning)
	{
		AbortAction();
	}

	OnFinished.Broadcast(this);
}

UWorld* UNPCAction::GetWorld() const
{
	// The CDO has no world; returning one here confuses the editor.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	if (const UObject* MyOuter = GetOuter())
	{
		return MyOuter->GetWorld();
	}

	return nullptr;
}
