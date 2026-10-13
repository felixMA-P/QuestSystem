// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Templates/SubclassOf.h"
#include "NPCActionDataAsset.generated.h"

class UNPCAction;

/**
 *
 * A configured NPC action: the action class to run, plus the values it runs with.
 *
 * Pick a class in Action Class and the details panel fills Action Defaults with that
 * class's own editable variables — Blueprint variables included, as long as they are
 * marked Instance Editable. Editing them here is what replaces hard-coding values into
 * the action Blueprint, so one action class can back many assets.
 *
 * At runtime the subsystem duplicates Action Defaults, so each run gets its own copy and
 * nothing an action does at runtime writes back into the asset.
 *
 * Felix Martin Arroyo
 */
UCLASS(BlueprintType)
class NPCMANAGERSYSTEM_API UNPCActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// The action to run. Changing this rebuilds Defaults with that class's variables.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Action")
	TSubclassOf<UNPCAction> ActionClass;

	// The values this action runs with — the properties shown here belong to ActionClass.
	// VisibleAnywhere locks the object itself (ActionClass is what picks it) while leaving
	// its own EditAnywhere properties editable.
	UPROPERTY(VisibleAnywhere, Instanced, Category = "NPC Action", meta = (DisplayName = "Action Defaults"))
	TObjectPtr<UNPCAction> Defaults;

	// Builds a runnable copy of Defaults outered to Outer. Null if the asset is unusable.
	// The returned action is not queued and has no completion binding yet; the subsystem adds that.
	UNPCAction* InstantiateAction(UObject* Outer) const;

	// True when ActionClass is set to something concrete.
	UFUNCTION(BlueprintPure, Category = "NPC Action")
	bool IsValidAction() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

#if WITH_EDITOR
	// Drops Defaults when ActionClass is empty/abstract, otherwise makes it match ActionClass.
	// Authoring-time only — an asset built in C++ at runtime just leaves Defaults null,
	// and InstantiateAction falls back to constructing ActionClass directly.
	void SyncDefaultsToActionClass();
#endif
};
