# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Unreal Engine 5.5 project implementing a modular quest system as a set of plugins. There is no game logic in the base `Source/` module — all meaningful code lives in the plugins.

## Build

Build via Unreal Editor or UnrealBuildTool from the engine installation:

```
# From the UE5 engine directory (adjust path to match local installation):
Engine\Build\BatchFiles\Build.bat QuestSystemEditor Win64 Development "A:\UE\Projects\QuestSystem\QuestSystem.uproject"
```

Hot-reload is supported inside the editor for iterative C++ changes. Generate project files via right-clicking the `.uproject` or using the editor's "File > Refresh Visual Studio Project".

## Plugin Architecture

Four plugins under `Plugins/`:

### QuestSystemPlugin (`Plugins/QuestSystemPlugin/`)
Split into two modules:
- **QuestSystemPluginRuntime** (`Source/QuestSystemPluginRuntime/`) — Runtime module. All gameplay-facing classes. Depends on `GameplayTags`.
- **QuestSystemPlugin** (`Source/QuestSystemPlugin/`) — Editor-only module. Custom asset editor for `UChainQuest` assets.

### NPCManagerSystem (`Plugins/NPCManagerSystem/`)
Single runtime module. Manages NPCs via `FGameplayTag` keys. (A `NPCManagerSystemEditor` module existed for the `UK2Node_RunNPCAction` Blueprint node; both were removed when actions moved to data assets.)

### DialogPlugin (`Plugins/DialogPlugin/`)
Newly added stub plugin. Minimal implementation.

### EventBusSystem (`Plugins/EventBusSystem/`)
Two runtime modules:
- **EventBusSystem** (`Source/EventBusSystem/`) — Core. Depends only on `Engine`, `GameplayTags`, `DeveloperSettings`, so it is usable standalone.
- **EventBusBridges** (`Source/EventBusBridges/`) — Optional integration with the other three plugins. Its `Build.cs` probes for each sibling plugin folder and links it only when present, defining `WITH_EVENTBUS_QUESTSYSTEM` / `WITH_EVENTBUS_NPCMANAGER` / `WITH_EVENTBUS_DIALOG`.

## Core Quest System Architecture

### Runtime Data Model

```
UDataAssetChainQuests          ← Data asset: array of UChainQuest* to load at once
└── UChainQuest                ← Blueprint/C++ subclassable UObject asset per quest chain
    ├── StartCondition         ← TSubclassOf<UQuestCondition>; nil = always starts
    ├── bUseSimpleConditions   ← When true, StartCondition/FQuestOutput::Condition (Blueprint classes) are ignored in favor of StartConditionTag/FQuestOutput::ConditionTag (FGameplayTag, exact-matched against QuestGameplayTagsContainer)
    ├── StartConditionTag      ← FGameplayTag; used instead of StartCondition when bUseSimpleConditions is true
    ├── bHasCalendarDates      ← Enables day-based expiry per quest node
    └── UChainQuestGraph       ← Runtime graph: array of UQuestRuntimeNode*
        └── UQuestRuntimeNode  ← Node holding UQuestInfoBase* + input/output UQuestRuntimePin*
            ├── UQuestInfo     ← Regular quest: Title, Description, DayToComplete,
            │                    OutPuts (TMap<TSubclassOf<UQuestCondition>, FText>)
            └── UEndQuestInfo  ← End quest: EndResult class, optional NextChainQuest
```

### Runtime Execution

`UQuestWorldSubsystem` (a `UWorldSubsystem`) owns all active quest state:
- **`ChainQuests`** — `TArray<FChainQuestHandler*>` for in-progress chain quests
- **`EndChainQuests`** — same, for completed ones
- **`QuestGameplayTagsContainer`** — `FGameplayTagContainer`; game systems call `AddGameplayTags()` to fire game events

`FChainQuestHandler` is a plain C++ class (not UObject) that tracks `CurrentNode` and `QuestsInfos` history for one `UChainQuest`. It is constructed by `UChainQuest::GetHandler()`.

**Condition evaluation flow** (`CheckOnGoingQuestConditions`):
1. For each active handler, call `FChainQuestHandler::CheckCurrentNodeConditions(World)`
2. That function iterates `UQuestInfo::OutPuts` keys in order; the first `UQuestCondition::CheckCondition()` that returns true advances `CurrentNode` along the matching output pin
3. Advancement recurses until an `EndNode` or no condition fires
4. On `EndNode`: `UEndQuestResult::ExecuteResult()` runs, optional `NextChainQuest` is queued

**Calendar flow** (`EndOfDay`):
- Increments `CurrentDay`, then: `CheckCalendar()` → `CheckOnGoingQuestConditions()` → `CheckCalendarOnGoingQuests()`
- Quests with `DayToComplete > 0` expire automatically by taking the first output pin

### Condition and Result Extension Points

Both are CDO-executed (called on `GetDefaultObject()`):
- `UQuestCondition` — override `CheckCondition(UWorld*)` in C++ or implement `CheckConditionEvent` in Blueprint
- `UEndQuestResult` — override `ExecuteResult(UWorld*)` in C++ or implement `ExecuteResultEvent` in Blueprint

Conditions for quest advancement live in `UQuestInfo::OutPuts` (key = condition class, value = display text). The order of map iteration determines pin matching.

Each `FQuestOutput` also carries a `ConditionTag` (`FGameplayTag`), used instead of `Condition` when the owning `UChainQuest::bUseSimpleConditions` is true — satisfied when that tag is present (exact match) in `UQuestWorldSubsystem::QuestGameplayTagsContainer`. This lets simple tag-gated advancement skip writing a `UQuestCondition` Blueprint subclass entirely.

### Blueprint Access Helpers

- `USystemFunctionHelper::GetQuestSystem(World)` → `UQuestWorldSubsystem*`
- `UFunctionHelpers::GetNpcManager(World)` → `UNpcManagerSubsystem*`

## Custom Asset Editor (Editor Module)

`FChainQuestAssetEditorApp` is a `FWorkflowCentricApplication` that provides the visual graph editor for `UChainQuest` assets.

Key relationship: the editor works on a `UEdGraph` (`WorkingGraph`) as the visual representation, and syncs to/from `UChainQuest::ChainQuestGraph` (the runtime `UChainQuestGraph`) on open and save.

Editor node hierarchy (all in `Source/QuestSystemPlugin/Public/Schemas/`):
- `UQuestGraphNodeBase` — base with shared pin creation interface
  - `UQuestStartGraphNode` — single output, no deletion
  - `UQuestGraphNode` — green title, dynamic output pins synced from `UQuestInfo::OutPuts`
  - `UQuestEndGraphNode` — terminal node

Graph schema: `UChainQuestGraphSchema` — allows only output→input connections; breaks existing connections on reconnect.

## NPC System

`UNpcManagerSubsystem` auto-registers actors on world initialization by scanning for `UNPCSystemComponent`, keeping a tag-keyed map of `AActor*`.

To register an NPC: add `UNPCSystemComponent` to the actor and set its `Tag` property. The subsystem populates automatically at world init and on actor spawn; manual `AddNPC` calls are also available.

`AddNPC` also looks up the matching `FNPCInfoRow` (matched on the row's `Tag` field) in the subsystem's `NPCDataTable` and caches it on the component.

### NPC Actions

`UNPCAction` is a Blueprint/C++-subclassable command run against one NPC (command pattern). Unlike `UQuestCondition`/`UQuestEvent`, actions are **instanced per invocation**, not CDO-executed — they hold per-run state (`TargetNPC`, `TargetTag`).

Actions are never run by class. `UNPCActionDataAsset` pairs a `TSubclassOf<UNPCAction> ActionClass` with `Defaults`, a `VisibleAnywhere, Instanced` instance of that class whose properties are the action's authored values. `PostEditChangeProperty` rebuilds `Defaults` whenever `ActionClass` changes, which is what makes the chosen subclass's variables appear in the asset. At runtime `InstantiateAction` duplicates `Defaults`, so each run gets a private copy. `UNPCAction` is `EditInlineNew` for this to work.

Each NPC gets a FIFO queue (`UNpcManagerSubsystem::ActionQueues`, keyed by tag): one action runs at a time, the next starts when the current calls `FinishAction()`. Queue with `RunActionOnNPC(Tag, Asset)` / `RunActionOnActor(Actor, Asset)`, or the `RunSoftAction*` variants that take a `TSoftObjectPtr` and load on demand.

Actions are not ticked. Durational work starts a `Delay`/timer/async node in `BeginAction` and calls `FinishAction` on completion; `UNPCAction::GetWorld()` is overridden so latent Blueprint nodes work. `StartNextAction` guards against re-entry via `bAdvancingQueue`, since an action may finish synchronously inside `BeginAction` or queue further actions.

## Event Bus System

`UEventBusSubsystem` (a `UWorldSubsystem`) is two layers that can each be used alone.

**Layer 1 — pub/sub.** `PostEvent(FGameEvent)` sends a tagged event carrying `Instigator`, `Target`, `ContextTags`, a `TMap<FGameplayTag, double> EventFacts`, and an `FInstancedStruct Payload`. Listeners register per channel tag with `Exact` or `IncludeChildren` matching (parent-chain walk, as Lyra's message subsystem does). Blueprint access is `UEventListenerComponent` and the `Listen For Game Event` async node.

**Layer 2 — rules.** `UEventRuleSet` assets hold `UEventRule` assets, which are bucketed by their `EventTag`. Per event: policy gate (ledger) → `Criteria` → `ContextQuery` → `CustomConditions`, then sort by `Priority` desc, `Specificity` desc (specificity = criteria + custom conditions + tag-query terms), weighted-random tie-break. The set's `SelectionMode` decides whether one rule wins (`BestMatch`) or all do (`AllMatching`). This is the rule-database pattern from Valve's dynamic dialog: a rule with no criteria is the fallback.

**Responses.** `UEventResponse` is `Abstract, EditInlineNew, Blueprintable` and mirrors `UNPCAction` exactly — `BeginResponse`/`FinishResponse`, `GetWorld()` overridden for latent nodes, duplicated per firing from the authored template. Rules run them FIFO with a `bAdvancingRun` re-entrancy guard, the same shape as `UNpcManagerSubsystem::StartNextAction`.

**Ledger.** `RuleRecords` (keyed by `UEventRule::RuleId`, an `FGuid` stamped into the asset) drives `Once` / `OncePerSubject` / `MaxCount` / `Cooldown`. `EventStats` answers `HasEventFired` / `GetEventCount` / `GetTimeSinceEvent`. `CaptureState()` / `RestoreState()` round-trip a plain `FEventBusSaveState`; the plugin ships no `USaveGame`. This is the only persistence anywhere in the repo.

**Dispatch safety.** Events posted during dispatch are queued, not recursed; chains deeper than `MaxChainDepth` are dropped with an error. Listener buckets hold `TSharedPtr` entries and are tombstoned rather than removed, so a callback may register or unregister listeners.

**Debugging.** `EventBus.Trace 1` logs every candidate rule and the exact reason it was rejected; also `EventBus.Post`, `EventBus.SetFact`, `EventBus.DumpFacts`, `EventBus.DumpLedger`, `EventBus.DumpRules`, `EventBus.History`, `EventBus.ResetLedger`.

## Gameplay Tags

Quest status tags registered natively via `FQuestTagsManager` singleton (initialized at plugin startup):
- `Gameplay.QuestSystem.Quest.Status.Enabled`
- `Gameplay.QuestSystem.Quest.Status.Disabled`
- `Gameplay.QuestSystem.Quest.Status.OnGoing`
- `Gameplay.QuestSystem.Quest.Status.Completed`

The primary condition mechanism is `UQuestWorldSubsystem::QuestGameplayTagsContainer` — game systems add tags here to represent world state, and `UQuestCondition` subclasses query it.

Event bus roots registered natively via `FEventBusTagsManager` (only the roots; project tags live under them):
- `Gameplay.EventBusSystem.Event`
- `Gameplay.EventBusSystem.Fact`
- `Gameplay.EventBusSystem.State`
