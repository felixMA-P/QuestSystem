# EventBusSystem

Tag-driven event bus with an authored rule layer. Two runtime modules: `EventBusSystem` (core, standalone) and `EventBusBridges` (optional integration with the other plugins).

Post a tagged event with data → rules match it against world state → responses run → the ledger records that they ran.

---

## Two layers

| Layer | What it does | Use it alone? |
|---|---|---|
| **Bus** | `PostEvent` sends a tagged event; listeners registered on that tag receive it. Publisher and listener never reference each other. | Yes — ignore rules entirely and it is a plain pub/sub subsystem. |
| **Rules** | Registered `UEventRule` assets are matched against each event; the winner runs its responses and the ledger records the fire. | Yes — nothing has to listen for rules to work. |

---

## Data

### FGameEvent

One thing that happened, plus the data that came with it.

| Property | Type | Description |
|---|---|---|
| `EventTag` | `FGameplayTag` | What happened. Listeners and rules are indexed by this |
| `Instigator` | `UObject*` | Who caused it. Also the subject key for `OncePerSubject` rules |
| `Target` | `UObject*` | Who it happened to, when that differs |
| `ContextTags` | `FGameplayTagContainer` | Flags for this occurrence, tested alongside the world's state tags |
| `EventFacts` | `TMap<FGameplayTag, double>` | Numbers a rule can test directly — no struct type needed on either side |
| `Payload` | `FInstancedStruct` | Arbitrary typed payload for listeners that share the struct. The rule layer does not look inside it |
| `EventId` | `int64` | Assigned on post, monotonic within a world |
| `TimeSeconds` | `float` | World time at post |

### FEventCriterion

One numeric test. A rule is a set of them, and how many it carries is its specificity.

| Property | Type | Description |
|---|---|---|
| `FactTag` | `FGameplayTag` | The fact to read |
| `Source` | `EEventFactSource` | `WorldFacts` (persistent) or `EventFacts` (this occurrence) |
| `Op` | `EEventCompareOp` | `Equal`, `NotEqual`, `Less`, `LessOrEqual`, `Greater`, `GreaterOrEqual`, `Exists`, `NotExists` |
| `Value` | `double` | Right-hand side; ignored by `Exists` / `NotExists` |

An unset fact reads as **absent, not zero**: every operator except `NotExists` fails on it.

---

## Assets

### UEventRule

"When this event happens, and the world looks like this, do these things — this often."

| Property | Type | Description |
|---|---|---|
| `EventTag` | `FGameplayTag` | The channel this rule listens on. Rules are bucketed by it |
| `MatchMode` | `EEventMatchMode` | `Exact`, or `IncludeChildren` so `Event.Combat` catches `Event.Combat.Kill.Headshot` |
| `Criteria` | `TArray<FEventCriterion>` | Numeric tests; all must pass |
| `ContextQuery` | `FGameplayTagQuery` | Tested against state tags ∪ the event's context tags |
| `CustomConditions` | `TArray<TSubclassOf<UEventCondition>>` | Hand-written tests, CDO-executed, run last |
| `Priority` | `int32` | Sorted on first, above specificity |
| `Weight` | `float` | Relative odds when equally specific rules tie |
| `bBlockLowerPriorityRules` | `bool` | In an `AllMatching` set, stop after this rule |
| `Policy` | `EEventFirePolicy` | `Always`, `Once`, `OncePerSubject`, `MaxCount`, `Cooldown` |
| `Responses` | `TArray<UEventResponse*>` | `Instanced`. Duplicated per firing, so authored values are never mutated |
| `bRunResponsesInSequence` | `bool` | Sequential (default) or all at once |
| `RuleId` | `FGuid` | Stable ledger key, stamped on creation. Survives renaming and moving the asset |

### UEventRuleSet

Rules registered and unregistered together, plus the selection mode they share.

| `SelectionMode` | Behaviour |
|---|---|
| `BestMatch` | The most specific matching rule runs; the rest are skipped. A rule with no criteria is the fallback |
| `AllMatching` | Every matching rule runs, in priority then specificity order |

Selection is **per set**, so a dialog-bark set and a quest set can each pick their own winner for the same event.

---

## Selection

For each posted event:

1. Gather candidate rules from the buckets along the event tag's parent chain.
2. **Policy gate first** — a spent `Once` rule costs one map lookup, not a full condition sweep.
3. Evaluate `Criteria` → `ContextQuery` → `CustomConditions` (cheapest first).
4. `Specificity = Criteria.Num() + CustomConditions.Num() + ContextQuery term count`.
5. Sort by `Priority` desc, then `Specificity` desc; break remaining ties by `Weight`.
6. Fire per the set's `SelectionMode`, recording each fire in the ledger.

---

## Responses

`UEventResponse` is `Abstract, EditInlineNew, Blueprintable` — the same shape as `UNPCAction`. Subclass in C++ (`BeginResponse`) or Blueprint (Begin Response event).

Durational work: start a Delay / timer / async node in `BeginResponse` and call `FinishResponse` when done. `GetWorld()` is overridden so latent Blueprint nodes work. A response that never finishes stalls the rest of its rule's list.

**Built in** (core, no dependencies): `Set Fact`, `Modify State Tags`, `Post Event`, `Delay`, `Print Debug`.

**Bridges** (`EventBusBridges`): `Add Quest Tags`, `Run NPC Action`, `Start Dialog`.

---

## Subsystem

`UEventBusSubsystem` (a `UWorldSubsystem`). Access via:

```cpp
UEventBusSubsystem* Bus = UEventBusFunctionHelper::GetEventBus(GetWorld());
```

| Group | Functions |
|---|---|
| Events | `PostEvent`, `PostEventByTag`, `RegisterListener` (C++), `UnregisterListener`, `UnregisterAllForObject`, `OnAnyEvent`, `OnRuleFired` |
| State tags | `AddStateTags`, `RemoveStateTags`, `HasStateTag`, `GetStateTags` |
| Facts | `SetFact`, `AddToFact`, `GetFact`, `HasFact`, `ClearFact`, `GetAllFacts` |
| Ledger | `HasEventFired`, `GetEventCount`, `GetTimeSinceEvent`, `GetRuleFireCount`, `HasRuleFired`, `GetRecentEvents`, `ResetRule`, `ResetLedger` |
| Rules | `RegisterRuleSet`, `UnregisterRuleSet`, `GetRegisteredRuleCount`, `SetTraceEnabled` |
| Persistence | `CaptureState`, `RestoreState` |
| Debug | `DumpDebugState`, `DumpRules` |

`HasEventFired` / `GetEventCount` / `GetTimeSinceEvent` are the queries the quest and dialog systems cannot express on their own — they answer "did this ever happen, how often, how long ago" and survive a save.

### Dispatch guarantees

- An event posted from inside a listener or a response is **queued behind** the current one, never recursed into.
- Chains deeper than `MaxChainDepth` (default 8) are dropped with an error, so a response that re-posts its own trigger fails loudly instead of hanging.
- A listener may register or unregister listeners from inside its own callback.

---

## Blueprint access

| Class | Use |
|---|---|
| `UEventBusFunctionHelper` | `Get Event Bus`, `Make Game Event` |
| `UEventListenerComponent` | Drop on an actor, set `ListenChannels`, bind `On Event Received` |
| `UAsyncAction_ListenForGameEvent` | `Listen For Game Event` node; `bOnlyTriggerOnce` or call `Cancel` |

---

## Persistence

The plugin ships **no** `USaveGame` and no slot management. It hands you a struct:

```cpp
FEventBusSaveState State = Bus->CaptureState();   // put it in your own save
Bus->RestoreState(State);                          // later
```

`FEventBusSaveState` holds `StateTags`, `Facts`, `RuleRecords` (keyed by `RuleId`), `EventStats` and `NextEventId`. All persisted properties are also marked `SaveGame`, so an `FObjectAndNameAsStringProxyArchive` picks them up for free.

`RestoreState` clamps saved timestamps to current world time — world time restarts at zero on level load, and untouched timestamps would otherwise make every cooldown look like it had just fired.

---

## Settings

**Project Settings → Plugins → Event Bus System** (`UEventBusSettings`, `DefaultGame.ini`):

| Setting | Default | Description |
|---|---|---|
| `AutoLoadedRuleSets` | empty | Rule sets registered on every world that creates the subsystem |
| `MaxEventHistory` | 64 | Recent events kept for debugging (not saved) |
| `MaxChainDepth` | 8 | How deep an event chain may go before it is treated as a loop |
| `bTraceMatching` | false | Log every candidate rule and why it matched or did not |

---

## Console commands

| Command | Description |
|---|---|
| `EventBus.Post <Tag>` | Post an event |
| `EventBus.SetFact <Tag> <Value>` | Set a numeric world fact |
| `EventBus.ClearFact <Tag>` | Remove a fact |
| `EventBus.AddStateTag <Tag>` | Set a boolean world-state flag |
| `EventBus.DumpFacts` | Log facts and state tags |
| `EventBus.DumpLedger` | Log event counts and per-rule fire records |
| `EventBus.DumpRules` | Log every registered rule by channel |
| `EventBus.History` | Log recent events and which rules they fired |
| `EventBus.Trace <0\|1>` | Per-event logging of every candidate rule and the exact reason it was rejected |
| `EventBus.ResetLedger` | Forget every recorded fire, so `Once` rules can run again |

`EventBus.Trace 1` is the one to reach for first when a rule "should have fired". It names the criterion, policy or condition that rejected it.

---

## Gameplay tags

Native roots registered at module startup (`FEventBusTagsManager`):

- `Gameplay.EventBusSystem.Event`
- `Gameplay.EventBusSystem.Fact`
- `Gameplay.EventBusSystem.State`

Project tags live under these by convention. Registering the roots natively means `IncludeChildren` matching and `FGameplayTagQuery` authoring work in an empty project.

---

## Bridges module

`EventBusBridges` links each sibling plugin **only when its folder is present** under `Plugins/`, so `EventBusSystem` still builds and ships alone. Detection happens in `EventBusBridges.Build.cs`, which defines `WITH_EVENTBUS_QUESTSYSTEM`, `WITH_EVENTBUS_NPCMANAGER` and `WITH_EVENTBUS_DIALOG`.

The bridge response classes always exist — UnrealHeaderTool cannot compile a `UCLASS` out behind a custom define — but their bodies are guarded. A bridge whose plugin was absent at build time logs a warning when it runs instead of silently doing nothing. Because their headers cannot name types from plugins that may not exist, asset properties are typed as `TSoftObjectPtr<UDataAsset>` / `TSoftObjectPtr<UObject>` and cast at runtime.

`Run NPC Action` returns as soon as the action is **queued**, not when it completes: `UNpcManagerSubsystem` already runs one action at a time per NPC, so waiting would only stack a second queue on top and let a slow walk-to stall an unrelated rule.

### Making quests and dialogs into event sources

The reverse direction is Blueprint wiring rather than C++, because binding a dynamic delegate needs a `UFUNCTION` whose signature names `UChainQuest` / `UDialogLineInfo` — types that do not exist when those plugins are absent, which would break the standalone build. In any Blueprint with access to both subsystems:

> Bind Event to `OnQuestAdvanced` → in the bound event, call `Post Event By Tag` with a tag of your choosing.

---

## Example: reactive bark

Goal — the first time the player kills a wolf while wounded, an NPC comments. Any other wolf kill gets a generic line.

1. Two rules, both on `Gameplay.EventBusSystem.Event.Combat.Kill`, in one `BestMatch` rule set.
2. **Specific rule**: criterion `Fact.Player.Health < 30`, `ContextQuery` requires `State.Enemy.Wolf`, `Policy = Once`, response `Run NPC Action` (a bark).
3. **Fallback rule**: no criteria, `Policy = Cooldown` at 30s, response `Print Debug`.
4. Game code posts:

```cpp
FGameEvent Event(Tag_CombatKill);
Event.Instigator = Killer;
Event.Target = Victim;
Event.ContextTags.AddTag(Tag_EnemyWolf);
Bus->PostEvent(Event);
```

Wounded → the specific rule wins on specificity. Healthy, or the second time → it falls through to the fallback. Neither rule knows the other exists.
