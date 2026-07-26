#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NPCAction.generated.h"

class UNPCAction;

/** Broadcast when an action finishes normally or is aborted. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNPCActionFinished, UNPCAction*, Action);

/** Subsystem-internal: lets the owning queue advance when an action completes. */
DECLARE_DELEGATE_OneParam(FNPCActionCompletedInternal, UNPCAction*);

/**
 *
 * A single command run against one NPC (command pattern).
 *
 * Subclass in C++ (override BeginAction) or in Blueprint (implement the Begin Action event).
 * One instance is created per invocation, so per-run state can live on the object.
 *
 * Durational actions: start the work in BeginAction — a Delay, a timer, an async node —
 * and call FinishAction when it is done. The instance is kept alive until then, and
 * GetWorld() is valid so latent Blueprint nodes work. An action that never calls
 * FinishAction will stall that NPC's queue.
 *
 * Felix Martin Arroyo
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class NPCMANAGERSYSTEM_API UNPCAction : public UObject
{
	GENERATED_BODY()

public:

	// The NPC this action operates on. Valid before BeginAction runs.
	UPROPERTY(BlueprintReadOnly, Category = "NPC Action")
	TObjectPtr<AActor> TargetNPC;

	// Tag the target NPC is registered under.
	UPROPERTY(BlueprintReadOnly, Category = "NPC Action")
	FGameplayTag TargetTag;

	// Fired on finish or abort. An action that finishes inside BeginAction broadcasts
	// before RunActionOnNPC returns, so for chaining prefer queue order over binding here.
	UPROPERTY(BlueprintAssignable, Category = "NPC Action")
	FNPCActionFinished OnFinished;

	// Call when the work is done; advances the NPC's queue. Ignored if already finished.
	UFUNCTION(BlueprintCallable, Category = "NPC Action")
	void FinishAction();

	UFUNCTION(BlueprintPure, Category = "NPC Action")
	FORCEINLINE bool IsRunning() const
	{
		return bRunning;
	}

	// Needed so latent Blueprint nodes (Delay) and timers work on this object.
	virtual UWorld* GetWorld() const override;

protected:

	// C++ entry point. Start the work here and call FinishAction when done.
	// The default implementation forwards to the Blueprint event.
	virtual void BeginAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "NPC Action", meta = (DisplayName = "Begin Action"))
	void BeginActionEvent();

	// Called instead of finishing when the action is cancelled or the world tears down.
	// Only called on actions that had already started.
	virtual void AbortAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "NPC Action", meta = (DisplayName = "Abort Action"))
	void AbortActionEvent();

private:

	friend class UNpcManagerSubsystem;

	// Subsystem-only.
	void Start(AActor* InTarget, const FGameplayTag& InTag);
	void Abort();

	// Subsystem-only completion hook. Not fired on abort — the subsystem drives that itself.
	FNPCActionCompletedInternal OnCompletedInternal;

	bool bRunning = false;
	bool bFinished = false;
};
