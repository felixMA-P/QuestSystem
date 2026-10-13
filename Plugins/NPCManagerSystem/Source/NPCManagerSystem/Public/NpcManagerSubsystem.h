// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NPCInfoRow.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Subsystems/WorldSubsystem.h"
#include "NpcManagerSubsystem.generated.h"

class UNPCAction;
class UNPCActionDataAsset;

/** One NPC's action queue: the action currently running, plus those waiting their turn. */
USTRUCT()
struct FNPCActionQueue
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UNPCAction> Running;

	UPROPERTY()
	TArray<TObjectPtr<UNPCAction>> Pending;
};

/**
 *
 * Registry of the world's NPCs, keyed by the gameplay tag on their UNPCSystemComponent,
 * plus a FIFO action queue per NPC.
 *
 * NPCs register themselves: at world initialization, and from their component's BeginPlay,
 * and unregister from its EndPlay. Each tag maps to exactly one actor.
 *
 * Felix Martin Arroyo
 */
UCLASS(Blueprintable, BlueprintType)
class NPCMANAGERSYSTEM_API UNpcManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// Registers the actor under its UNPCSystemComponent's tag. Does nothing if it is already
	// registered; refused, with an error, if another live actor already holds that tag.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void AddNPC(AActor* NPC);

	// Unregisters whatever is registered under this tag and cancels its actions.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void RemoveNPC(const FGameplayTag NameTag);

	// Unregisters this actor, if it is registered, and cancels its actions. Unlike RemoveNPC
	// this never touches another actor that has since taken the same tag.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void RemoveNPCActor(AActor* NPC);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	AActor* GetNPC(const FGameplayTag NameTag);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void MoveNPC(const FGameplayTag NameTag, const FVector& Location);

	// Looks up the NPCDataTable row whose Tag field matches. Returns false if not found.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	bool GetNPCInfo(const FGameplayTag& Tag, FNPCInfoRow& OutInfo);

	// Builds a configured action from the asset without queueing it, so the instance can be
	// tweaked on top of the asset's values first. Queue it in the same call stack;
	// an unqueued action is not kept alive.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	UNPCAction* CreateNPCAction(const UNPCActionDataAsset* ActionAsset);

	// Queues an action built by CreateNPCAction, starting it if that NPC is idle.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	bool QueueAction(const FGameplayTag NPCTag, UNPCAction* Action);

	// Create + queue in one call. Returns the new instance, or null if the NPC or asset is invalid.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	UNPCAction* RunActionOnNPC(const FGameplayTag NPCTag, const UNPCActionDataAsset* ActionAsset);

	// As RunActionOnNPC, resolving the tag from the actor's UNPCSystemComponent.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	UNPCAction* RunActionOnActor(AActor* NPC, const UNPCActionDataAsset* ActionAsset);

	// Soft-reference variants: load the asset on demand, so a quest, dialog or level Blueprint
	// can name an action without pulling it — and everything it references — into memory up front.
	// The load is synchronous; call these off the critical path if the asset is heavy.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	UNPCAction* RunSoftActionOnNPC(const FGameplayTag NPCTag, TSoftObjectPtr<UNPCActionDataAsset> ActionAsset);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	UNPCAction* RunSoftActionOnActor(AActor* NPC, TSoftObjectPtr<UNPCActionDataAsset> ActionAsset);

	// Aborts the running action and discards everything queued behind it for this NPC.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	void CancelNPCActions(const FGameplayTag NPCTag);

	UFUNCTION(BlueprintPure, Category = "NPCManagerSystem|Actions")
	UNPCAction* GetRunningAction(const FGameplayTag NPCTag) const;

	// Number of actions waiting behind the running one.
	UFUNCTION(BlueprintPure, Category = "NPCManagerSystem|Actions")
	int32 GetQueuedActionCount(const FGameplayTag NPCTag) const;

private:

	// DataTable of FNPCInfoRow, loaded in Initialize from UNPCManagerSettings. Rows are matched on their Tag field.
	UPROPERTY()
	TObjectPtr<UDataTable> NPCDataTable;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AActor>> TagNPCsMap;

	// Holds action instances alive while queued or running.
	UPROPERTY()
	TMap<FGameplayTag, FNPCActionQueue> ActionQueues;

	// True while StartNextAction is walking a queue, so synchronous completions
	// fall back into its loop instead of recursing.
	bool bAdvancingQueue = false;

	FDelegateHandle OnActorsInitializedDelegate;

	// Silent lookup, for registration where a missing row is normal.
	const FNPCInfoRow* FindNPCInfoRow(const FGameplayTag& Tag) const;

protected:

	void InitNpcList(const FActorsInitializedParams& ActorsInitializedParams);

	// Starts the next pending action for this NPC, if any, and prunes the queue when empty.
	void StartNextAction(const FGameplayTag& NPCTag);

	// Bound to each action's internal completion hook.
	void HandleActionCompleted(UNPCAction* Action);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

};
