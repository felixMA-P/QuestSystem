# NPCManageSystem

Tag-keyed registries for NPCs and world entities. Single runtime module: `NPCManageSystem`.

Both subsystems auto-populate at world initialization by scanning all actors for their respective components. Manual registration is also available.

---

## Components

### UNPCSystemComponent

Add to any Actor to make it register with `UNpcManagerSubsystem`.

| Property | Type | Category | Description |
|---|---|---|---|
| `Tag` | `FGameplayTag` | NPC System | Unique key used to look up this NPC in the subsystem |

### UEntityComponent

Add to any Actor to make it register with `UEntitiesSubsystem`.

| Property | Type | Category | Description |
|---|---|---|---|
| `Tag` | `FGameplayTag` | Entity System | Unique key used to look up this entity in the subsystem |

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
| `AddNPC(Actor)` | NPCManageSystem | Registers an actor using the `FGameplayTag` from its `UNPCSystemComponent` |
| `RemoveNPC(NameTag)` | NPCManageSystem | Removes the NPC registered under the given tag |
| `GetNPC(NameTag)` | NPCManageSystem | Returns the actor registered under the given tag, or null |
| `MoveNPC(NameTag, Location)` | NPCManageSystem | Teleports the registered NPC to the given world location |

#### Auto-Registration

On world initialization the subsystem scans all placed actors for `UNPCSystemComponent` and calls `RegisterNPC` on each. Actors spawned after world init must be registered manually via `AddNPC`.

---

### UEntitiesSubsystem

`UWorldSubsystem` that maintains a tag-keyed map of world entities (items, interactables, etc.). Access via:

```cpp
UEntitiesSubsystem* ES = UFunctionHelpers::GetEntitiesManager(GetWorld());
```

#### Functions

| Function | Category | Description |
|---|---|---|
| `AddItem(Actor)` | EntitiesSystem | Registers an actor using the `FGameplayTag` from its `UEntityComponent` |
| `RemoveItem(EntityTag)` | EntitiesSystem | Removes the entity registered under the given tag |
| `GetItem(EntityTag)` | EntitiesSystem | Returns the actor registered under the given tag, or null |

#### Auto-Registration

Same pattern as `UNpcManagerSubsystem` — scans for `UEntityComponent` at world init.

---

## Blueprint Access

```cpp
UFunctionHelpers::GetNpcManager(World)       →  UNpcManagerSubsystem*
UFunctionHelpers::GetEntitiesManager(World)  →  UEntitiesSubsystem*
```

Both helpers are `BlueprintPure` and `BlueprintCallable` under the `"CustomSubsystemsAccess"` category.

---

## Adding an NPC or Entity

1. Add `UNPCSystemComponent` (NPC) or `UEntityComponent` (Entity) to the actor.
2. Set the component's `Tag` property to a unique `FGameplayTag`.
3. If the actor is placed in the level before play, the subsystem registers it automatically. If spawned at runtime, call `AddNPC` / `AddItem` manually.

```cpp
// Runtime spawn example
AActor* SpawnedNPC = GetWorld()->SpawnActor<AActor>(NPCClass, Transform);
UFunctionHelpers::GetNpcManager(GetWorld())->AddNPC(SpawnedNPC);
```
