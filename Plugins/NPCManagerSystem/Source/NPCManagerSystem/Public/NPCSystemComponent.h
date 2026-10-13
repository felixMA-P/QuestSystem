// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NPCInfoRow.h"
#include "NPCSystemComponent.generated.h"

/**
 *
 * Marks its owner as an NPC and gives it the tag the NPC Manager knows it by.
 *
 * The owner registers with UNpcManagerSubsystem on BeginPlay (placed actors are also registered
 * earlier, at world initialization) and unregisters on EndPlay, so destroyed or streamed-out
 * NPCs never linger in the registry. Change the tag at runtime with SetTag.
 *
 * Felix Martin Arroyo
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPCMANAGERSYSTEM_API UNPCSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNPCSystemComponent();

	// Must be unique among the world's NPCs.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "NPC System")
	FGameplayTag Tag;

private:
	UPROPERTY()
	FNPCInfoRow NPCData;

public:

	// Re-registers the owner under NewTag. Any actions queued on it under the old tag are cancelled.
	UFUNCTION(BlueprintCallable, Category = "NPC System")
	void SetTag(const FGameplayTag NewTag);

	// The NPC Info DataTable row matching Tag; empty if the table has none.
	UFUNCTION(BlueprintPure, Category = "NPC System")
	FORCEINLINE FNPCInfoRow GetNPCData() const
	{
		return NPCData;
	}

	FORCEINLINE void SetNPCData(const FNPCInfoRow& InNPCData)
	{
		NPCData = InNPCData;
	}

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
