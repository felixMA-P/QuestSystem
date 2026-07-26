
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NPCInfoRow.h"
#include "Engine/DataTable.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "NpcManagerSubsystem.generated.h"

class ANPC;
class UNPCAction;

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
 * This system have the count of the Main Characters of the game and the secondary Characters
 * 
 * Felix Martin Arroyo
 */
UCLASS(Blueprintable, BlueprintType)
class NPCMANAGERSYSTEM_API UNpcManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void AddNPC(AActor* NPC);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void RemoveNPC(const FGameplayTag NameTag);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	AActor* GetNPC(const FGameplayTag NameTag);

	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	void MoveNPC(const FGameplayTag NameTag, const FVector& Location);

	// Looks up the NPCDataTable row whose Tag field matches. Returns false if not found.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem")
	bool GetNPCInfo(const FGameplayTag& Tag, FNPCInfoRow& OutInfo);

	// Creates an action without queueing it, so its properties can be set first.
	// Configure and queue it in the same call stack; an unqueued action is not kept alive.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions", meta = (DeterminesOutputType = "ActionClass"))
	UNPCAction* CreateNPCAction(TSubclassOf<UNPCAction> ActionClass);

	// Queues an action built by CreateNPCAction, starting it if that NPC is idle.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	bool QueueAction(const FGameplayTag NPCTag, UNPCAction* Action);

	// Create + queue in one call, for actions that need no configuration.
	// Returns the new instance, or null if the NPC or class is invalid.
	// Hidden from the palette: the UK2Node_RunNPCAction node is the Blueprint entry point,
	// since it can also expose the action class's Expose-on-Spawn properties as pins.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions",
		meta = (DeterminesOutputType = "ActionClass", BlueprintInternalUseOnly = "true"))
	UNPCAction* RunActionOnNPC(const FGameplayTag NPCTag, TSubclassOf<UNPCAction> ActionClass);

	// As RunActionOnNPC, resolving the tag from the actor's UNPCSystemComponent.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions", meta = (DeterminesOutputType = "ActionClass"))
	UNPCAction* RunActionOnActor(AActor* NPC, TSubclassOf<UNPCAction> ActionClass);

	// Aborts the running action and discards everything queued behind it for this NPC.
	UFUNCTION(BlueprintCallable, Category = "NPCManagerSystem|Actions")
	void CancelNPCActions(const FGameplayTag NPCTag);

	UFUNCTION(BlueprintPure, Category = "NPCManagerSystem|Actions")
	UNPCAction* GetRunningAction(const FGameplayTag NPCTag) const;

	// Number of actions waiting behind the running one.
	UFUNCTION(BlueprintPure, Category = "NPCManagerSystem|Actions")
	int32 GetQueuedActionCount(const FGameplayTag NPCTag) const;

private:

	// DataTable of FNPCInfoRow, loaded in Initialize. Rows are matched on their Tag field.
	UPROPERTY()
	TObjectPtr<UDataTable> NPCDataTable;

	UPROPERTY()
	TMap<FGameplayTag, AActor*> TagNPCsMap;

	// Holds action instances alive while queued or running.
	UPROPERTY()
	TMap<FGameplayTag, FNPCActionQueue> ActionQueues;

	// True while StartNextAction is walking a queue, so synchronous completions
	// fall back into its loop instead of recursing.
	bool bAdvancingQueue = false;

	FDelegateHandle OnActorsInitializedDelegate;

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
