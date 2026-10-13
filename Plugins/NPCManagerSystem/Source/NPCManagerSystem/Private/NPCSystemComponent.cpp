// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#include "NPCSystemComponent.h"

#include "Engine/World.h"
#include "NpcManagerSubsystem.h"


UNPCSystemComponent::UNPCSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UNPCSystemComponent::SetTag(const FGameplayTag NewTag)
{
	if (NewTag == Tag)
	{
		return;
	}

	UNpcManagerSubsystem* Manager = GetWorld() ? GetWorld()->GetSubsystem<UNpcManagerSubsystem>() : nullptr;

	if (Manager)
	{
		Manager->RemoveNPCActor(GetOwner());
	}

	Tag = NewTag;

	// Before BeginPlay, registration will pick the new tag up on its own.
	if (Manager && HasBegunPlay())
	{
		Manager->AddNPC(GetOwner());
	}
}

void UNPCSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Covers actors spawned after world initialization, and tags set in a construction
	// script or before FinishSpawning. A no-op if world initialization already registered us.
	if (UNpcManagerSubsystem* Manager = GetWorld()->GetSubsystem<UNpcManagerSubsystem>())
	{
		Manager->AddNPC(GetOwner());
	}
}

void UNPCSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Destroyed, streamed out, or the world is ending: either way the registry must not keep us.
	if (UNpcManagerSubsystem* Manager = GetWorld() ? GetWorld()->GetSubsystem<UNpcManagerSubsystem>() : nullptr)
	{
		Manager->RemoveNPCActor(GetOwner());
	}

	Super::EndPlay(EndPlayReason);
}
