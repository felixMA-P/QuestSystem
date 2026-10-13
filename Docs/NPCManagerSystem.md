# NPCManagerSystem

Tag-keyed registry for NPCs. Single runtime module: `NPCManagerSystem`.

NPCs register themselves through `UNPCSystemComponent` and unregister when they leave play. Manual registration is also available.

---

## Components

### UNPCSystemComponent

Add to any Actor to make it register with `UNpcManagerSubsystem`.

| Property | Type | Category | Description |
|---|---|---|---|
| `Tag` | `FGameplayTag` | NPC System | Unique key used to look up this NPC in the subsystem |

| Function | Description |
|---|---|
| `SetTag(NewTag)` | Re-registers the owner under a new tag. Actions queued under the old tag are cancelled |
| `GetNPCData()` | `BlueprintPure`. The NPC Info row matching `Tag`, or an empty row if the table has none |

The component registers its owner on `BeginPlay` and unregisters it on `EndPlay`, so destroyed and streamed-out NPCs never linger in the registry and their queued actions are cancelled.

---

## Settings

**Project Settings → Plugins → NPC Manager System** (`UNPCManagerSettings`, saved to the project's `DefaultGame.ini`).

| Property | Type | Description |
|---|---|---|
| `NPCDataTable` | `TSoftObjectPtr<UDataTable>` | Table of `FNPCInfoRow` to read display data from. Defaults to the example table shipped with the plugin, `/NPCManagerSystem/NPCManagerSystem/Data/DT_NPCInfo`. Leave empty if NPCs need no display data |

Point this at your own table rather than editing the plugin's copy, which a plugin update would overwrite. Whichever table is set is added to the cook automatically, so it does not need to be referenced by anything. A table whose row structure is not `NPCInfoRow` is ignored with an error.

---

## Data

### FNPCInfoRow

`FTableRowBase` struct describing an NPC. Rows are matched by their `Tag` field, not by row name.

| Property | Type | Description |
|---|---|---|
| `Tag` | `FGameplayTag` | Identifying tag; matched against the component's `Tag` |
| `Mesh3D` | `TSoftObjectPtr<USkeletalMesh>` | Skeletal mesh for 3D representation |
| `Image2D` | `TSoftObjectPtr<UTexture2D>` | Portrait / icon |
| `Name` | `FText` | Display name |
| `Description` | `FText` | Display description |

---

## Subsystems

### UNpcManagerSubsystem

`UWorldSubsystem` that maintains a tag-keyed map of NPC actors. Access via:

```cpp
UNpcManagerSubsystem* NM = UFunctionHelpers::GetNpcManager(GetWorld());
```

#### Functions

| Function | Category | Description |
|---|---|---|
| `AddNPC(Actor)` | NPCManagerSystem | Registers an actor using the `FGameplayTag` from its `UNPCSystemComponent`, and pushes the matching DataTable row into that component. Re-adding the same actor does nothing; a tag already held by another live actor is refused with an error |
| `RemoveNPC(NameTag)` | NPCManagerSystem | Removes the NPC registered under the given tag and cancels its actions |
| `RemoveNPCActor(Actor)` | NPCManagerSystem | Removes this actor, whatever tag it is registered under, and cancels its actions |
| `GetNPC(NameTag)` | NPCManagerSystem | Returns the actor registered under the given tag, or null (also null for an actor being destroyed) |
| `MoveNPC(NameTag, Location)` | NPCManagerSystem | Teleports the registered NPC to the given world location; logs and does nothing if the tag is unknown |
| `GetNPCInfo(Tag, OutInfo)` | NPCManagerSystem | Looks up the `FNPCInfoRow` whose `Tag` matches; returns false if not found |

#### Auto-Registration

- `OnActorsInitialized` → `InitNpcList` registers every actor placed in the level that has a `UNPCSystemComponent`. This runs before any `BeginPlay`, so placed NPCs can be looked up from the very start.
- `UNPCSystemComponent::BeginPlay` registers actors spawned later, and picks up tags set in a construction script or before `FinishSpawning`.
- `UNPCSystemComponent::EndPlay` unregisters the actor (destroy, level streaming, world teardown).

Actors without a valid `Tag` are skipped and logged as an error. Each tag maps to exactly one actor.

---

## Actions (Command Pattern)

`UNPCAction` is a command run against one NPC. Subclass it in C++ (override `BeginAction`) or in Blueprint (implement the **Begin Action** event). One instance is created per invocation, so per-run state can live on the object.

Actions are never run by class. Every entry point takes a `UNPCActionDataAsset`, which names the class *and* carries the values it runs with — see [Configuring an action](#configuring-an-action-unpcactiondataasset).

### Lifecycle

| Member | Description |
|---|---|
| `TargetNPC` | `AActor*` the action operates on; valid before `BeginAction` runs |
| `TargetTag` | Tag the target is registered under |
| `BeginAction()` / `Begin Action` | Entry point. Start the work here |
| `FinishAction()` | Call when done. Advances the NPC's queue. Ignored if already finished |
| `AbortAction()` / `Abort Action` | Called instead of finishing when cancelled or on world teardown. Only for actions that had started |
| `OnFinished` | `BlueprintAssignable`, fired on finish *or* abort |
| `IsRunning()` | True between `Start` and completion |

Actions are **not** ticked. For anything durational, start a `Delay`, a timer, or an async node in `BeginAction` and call `FinishAction` from its completion. `GetWorld()` is overridden so latent Blueprint nodes work, and the instance is kept alive by the subsystem until it finishes.

> An action that never calls `FinishAction` stalls that NPC's queue permanently.

### Configuring an action: UNPCActionDataAsset

A `UDataAsset` holding one configured action.

| Property | Type | Description |
|---|---|---|
| `ActionClass` | `TSubclassOf<UNPCAction>` | The action to run |
| `Defaults` ("Action Defaults") | `UNPCAction*` (`Instanced`) | The values it runs with — these are `ActionClass`'s own variables |

Pick a class in **Action Class** and **Action Defaults** fills with that class's editable variables, Blueprint variables included as long as they are marked *Instance Editable*. `Defaults` is `VisibleAnywhere`, so the object itself cannot be swapped from the details panel — `ActionClass` is what chooses it — while its properties stay editable.

Values therefore live in the asset, not baked into the action class, so one action class backs as many differently-configured assets as you need. At runtime the subsystem duplicates `Defaults`, so each run gets a private copy and nothing an action writes at runtime leaks back into the asset.

To create one: **Content Browser → Miscellaneous → Data Asset → NPCActionDataAsset**.

```cpp
// Reading authored values inside the action.
void UMoveToForgeAction::BeginAction()
{
    TargetNPC->SetActorLocation(Destination);   // Destination was set on the asset
    FinishAction();
}
```

> Changing `ActionClass` on an existing asset rebuilds `Defaults` from scratch — values authored for the previous class are gone. It is undoable.

### Queueing

Each NPC has its own FIFO queue — one action runs at a time, the next starts when the current one finishes. `RunActionOnNPC` returns immediately; the action starts right away if that NPC is idle, otherwise it waits its turn.

| Function | Description |
|---|---|
| `CreateNPCAction(ActionAsset)` | Builds a configured action *without* queueing it, so the instance can be tweaked on top of the asset's values |
| `QueueAction(NPCTag, Action)` | Queues an action built by `CreateNPCAction`; starts it if the NPC is idle |
| `RunActionOnNPC(NPCTag, ActionAsset)` | Create + queue in one call |
| `RunActionOnActor(NPC, ActionAsset)` | Same, resolving the tag from the actor's `UNPCSystemComponent` |
| `RunSoftActionOnNPC(NPCTag, ActionAsset)` | As above, taking a `TSoftObjectPtr` and loading it synchronously |
| `RunSoftActionOnActor(NPC, ActionAsset)` | Soft-reference form of `RunActionOnActor` |
| `CancelNPCActions(NPCTag)` | Aborts the running action and discards everything queued behind it |
| `GetRunningAction(NPCTag)` | The action currently running on that NPC, or null |
| `GetQueuedActionCount(NPCTag)` | How many are waiting behind the running one |

Most callers want `RunActionOnNPC` — the asset already carries the configuration, so there is nothing to set up afterwards. Reach for `CreateNPCAction` + `QueueAction` only to override an asset value for one particular run. Configure and queue within the same call stack; an action that has been created but not queued is not referenced by the subsystem and so is not protected from garbage collection.

The soft variants exist so a quest, dialog or level Blueprint can name an action without pulling it — and everything it references — into memory up front. They load synchronously, so keep them off the critical path when the asset is heavy.

Queues are cleaned up automatically: `RemoveNPC` cancels that NPC's actions, and `Deinitialize` aborts everything still in flight.

### Example

```cpp
// Walk to the forge, then hammer — the second waits for the first.
UNpcManagerSubsystem* NM = UFunctionHelpers::GetNpcManager(GetWorld());
NM->RunActionOnNPC(BlacksmithTag, MoveToForgeAsset);
NM->RunActionOnNPC(BlacksmithTag, HammerAsset);
```

```cpp
// Overriding one authored value for a single run.
UNPCAction* Move = NM->CreateNPCAction(MoveToForgeAsset);

if (UMoveToForgeAction* Typed = Cast<UMoveToForgeAction>(Move))
{
    Typed->Destination = AlternateForge;
}

NM->QueueAction(BlacksmithTag, Move);
```

Because an action that finishes synchronously inside `BeginAction` broadcasts `OnFinished` before `RunActionOnNPC` returns, prefer queue order over binding to `OnFinished` when chaining.

```cpp
// A durational C++ action
void UMoveToForgeAction::BeginAction()
{
    TargetNPC->SetActorLocation(Destination);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle,
        FTimerDelegate::CreateUObject(this, &UMoveToForgeAction::FinishAction), 2.0f, false);
}
```

---

## Blueprint Access

```cpp
UFunctionHelpers::GetNpcManager(World)  →  UNpcManagerSubsystem*
```

`BlueprintPure` and `BlueprintCallable` under the `"CustomSubsystemsAccess"` category.

---

## Adding an NPC

1. Add `UNPCSystemComponent` to the actor.
2. Set the component's `Tag` property to a unique `FGameplayTag`.
3. If the NPC needs display data, add a row with the same `Tag` to the table set in **Project Settings → Plugins → NPC Manager System**.

Registration is automatic whether the actor is placed in the level or spawned at runtime. To register explicitly:

```cpp
AActor* SpawnedNPC = GetWorld()->SpawnActor<AActor>(NPCClass, Transform);
UFunctionHelpers::GetNpcManager(GetWorld())->AddNPC(SpawnedNPC);
```
