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
