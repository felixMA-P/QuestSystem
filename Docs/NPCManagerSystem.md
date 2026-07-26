# NPCManagerSystem

Tag-keyed registry for NPCs. Single runtime module: `NPCManagerSystem`.

The subsystem auto-populates at world initialization by scanning all actors for `UNPCSystemComponent`. Manual registration is also available.

---

## Components

### UNPCSystemComponent

Add to any Actor to make it register with `UNpcManagerSubsystem`.

| Property | Type | Category | Description |
|---|---|---|---|
| `Tag` | `FGameplayTag` | NPC System | Unique key used to look up this NPC in the subsystem |

The component also caches an `FNPCInfoRow` (private), populated by the subsystem during registration. Read it via `GetNPCData()`.

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
| `AddNPC(Actor)` | NPCManagerSystem | Registers an actor using the `FGameplayTag` from its `UNPCSystemComponent`, and pushes the matching DataTable row into that component |
| `RemoveNPC(NameTag)` | NPCManagerSystem | Removes the NPC registered under the given tag |
| `GetNPC(NameTag)` | NPCManagerSystem | Returns the actor registered under the given tag, or null |
| `MoveNPC(NameTag, Location)` | NPCManagerSystem | Teleports the registered NPC to the given world location |
| `GetNPCInfo(Tag, OutInfo)` | NPCManagerSystem | Looks up the `FNPCInfoRow` whose `Tag` matches; returns false if not found |

#### Auto-Registration

`Initialize` binds two world delegates:

- `OnActorsInitialized` → `InitNpcList`, which scans every actor in the world and passes each to `AddNPC` (non-NPC actors are filtered out by the missing component).
- `AddOnActorSpawnedHandler` → `RegisterNPC`, so actors spawned after world init register automatically.

Actors without a valid `Tag` are skipped, and logged as an error in editor builds.

---

## Actions (Command Pattern)

`UNPCAction` is a command run against one NPC. Subclass it in C++ (override `BeginAction`) or in Blueprint (implement the **Begin Action** event). One instance is created per invocation, so per-run state can live on the object.

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

### Queueing

Each NPC has its own FIFO queue — one action runs at a time, the next starts when the current one finishes. `RunActionOnNPC` returns immediately; the action starts right away if that NPC is idle, otherwise it waits its turn.

| Function | Description |
|---|---|
| `CreateNPCAction(ActionClass)` | Builds an action *without* queueing it, so its properties can be set first |
| `QueueAction(NPCTag, Action)` | Queues an action built by `CreateNPCAction`; starts it if the NPC is idle |
| `RunActionOnNPC(NPCTag, ActionClass)` | Create + queue in one call, for actions needing no configuration |
| `RunActionOnActor(NPC, ActionClass)` | Same, resolving the tag from the actor's `UNPCSystemComponent` |
| `CancelNPCActions(NPCTag)` | Aborts the running action and discards everything queued behind it |
| `GetRunningAction(NPCTag)` | The action currently running on that NPC, or null |
| `GetQueuedActionCount(NPCTag)` | How many are waiting behind the running one |

Use `CreateNPCAction` + `QueueAction` whenever an action needs parameters — `RunActionOnNPC` starts the action immediately, leaving no window to configure it. Configure and queue within the same call stack; an action that has been created but not queued is not referenced by the subsystem and so is not protected from garbage collection.

### UNPCMoveToPointAction

Moves the NPC in a straight line from wherever it stands to `Destination`, driven by a repeating timer.

| Property | Default | Description |
|---|---|---|
| `Destination` | `(0,0,0)` | World-space target |
| `Speed` | `200.0` | Units per second; ignored when `Duration` is set |
| `Duration` | `0.0` | Above zero, the trip takes exactly this long regardless of distance |
| `bOrientToDirection` | `true` | Face the direction of travel when the move starts |
| `bSweep` | `true` | Collide with blocking geometry instead of passing through |
| `UpdateInterval` | `0.016` | Seconds between position updates |

No NavMesh, `AIController` or `AIModule` dependency required — but it does not path around obstacles. With `bSweep` on it stops against blocking geometry, and the action still finishes on schedule, so the NPC may end up short of `Destination`.

```cpp
UNpcManagerSubsystem* NM = UFunctionHelpers::GetNpcManager(GetWorld());

UNPCMoveToPointAction* Move =
    Cast<UNPCMoveToPointAction>(NM->CreateNPCAction(UNPCMoveToPointAction::StaticClass()));

Move->Destination = ForgeLocation;
Move->Speed = 300.0f;

NM->QueueAction(BlacksmithTag, Move);
```

Queues are cleaned up automatically: `RemoveNPC` cancels that NPC's actions, and `Deinitialize` aborts everything still in flight.

### Example

```cpp
// Walk to the forge, then hammer — the second waits for the first.
UNpcManagerSubsystem* NM = UFunctionHelpers::GetNpcManager(GetWorld());
NM->RunActionOnNPC(BlacksmithTag, UMoveToForgeAction::StaticClass());
NM->RunActionOnNPC(BlacksmithTag, UHammerAction::StaticClass());
```

```cpp
// A durational C++ action
void UMoveToForgeAction::BeginAction()
{
    TargetNPC->SetActorLocation(ForgeLocation);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle,
        FTimerDelegate::CreateUObject(this, &UMoveToForgeAction::FinishAction), 2.0f, false);
}
```

Because an action that finishes synchronously inside `BeginAction` broadcasts `OnFinished` before `RunActionOnNPC` returns, prefer queue order over binding to `OnFinished` when chaining.

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
3. Add a row to the NPC DataTable with the same `Tag` if the NPC needs display data.

Registration is automatic whether the actor is placed in the level or spawned at runtime. To register explicitly:

```cpp
AActor* SpawnedNPC = GetWorld()->SpawnActor<AActor>(NPCClass, Transform);
UFunctionHelpers::GetNpcManager(GetWorld())->AddNPC(SpawnedNPC);
```
