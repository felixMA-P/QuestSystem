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

Three plugins under `Plugins/`:

### QuestSystemPlugin (`Plugins/QuestSystemPlugin/`)
Split into two modules:
- **QuestSystemPluginRuntime** (`Source/QuestSystemPluginRuntime/`) — Runtime module. All gameplay-facing classes. Depends on `GameplayTags`.
- **QuestSystemPlugin** (`Source/QuestSystemPlugin/`) — Editor-only module. Custom asset editor for `UChainQuest` assets.

### NPCManageSystem (`Plugins/NPCManageSystem/`)
Single runtime module. Manages NPCs and world entities via `FGameplayTag` keys.

### DialogPlugin (`Plugins/DialogPlugin/`)
Newly added stub plugin. Minimal implementation.

## Core Quest System Architecture

### Runtime Data Model

```
UDataAssetChainQuests          ← Data asset: array of UChainQuest* to load at once
└── UChainQuest                ← Blueprint/C++ subclassable UObject asset per quest chain
    ├── StartCondition         ← TSubclassOf<UQuestCondition>; nil = always starts
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

### Blueprint Access Helpers

- `USystemFunctionHelper::GetQuestSystem(World)` → `UQuestWorldSubsystem*`
- `UFunctionHelpers::GetNpcManager(World)` → `UNpcManagerSubsystem*`
- `UFunctionHelpers::GetEntitiesManager(World)` → `UEntitiesSubsystem*`

## Custom Asset Editor (Editor Module)

`FChainQuestAssetEditorApp` is a `FWorkflowCentricApplication` that provides the visual graph editor for `UChainQuest` assets.

Key relationship: the editor works on a `UEdGraph` (`WorkingGraph`) as the visual representation, and syncs to/from `UChainQuest::ChainQuestGraph` (the runtime `UChainQuestGraph`) on open and save.

Editor node hierarchy (all in `Source/QuestSystemPlugin/Public/Schemas/`):
- `UQuestGraphNodeBase` — base with shared pin creation interface
  - `UQuestStartGraphNode` — single output, no deletion
  - `UQuestGraphNode` — green title, dynamic output pins synced from `UQuestInfo::OutPuts`
  - `UQuestEndGraphNode` — terminal node

Graph schema: `UChainQuestGraphSchema` — allows only output→input connections; breaks existing connections on reconnect.

## NPC and Entity Systems

Both subsystems auto-register actors on world initialization by scanning for their respective components:
- **`UNpcManagerSubsystem`** — finds actors with `UNPCSystemComponent`; tag-keyed map of `AActor*`
- **`UEntitiesSubsystem`** — finds actors with `UEntityComponent`; tag-keyed map of `AActor*`

To register an NPC or entity: add `UNPCSystemComponent` or `UEntityComponent` to the actor and set its `Tag` property. The subsystem populates automatically at world init; manual `AddNPC`/`AddItem` calls are also available.

## Gameplay Tags

Quest status tags registered natively via `FQuestTagsManager` singleton (initialized at plugin startup):
- `Gameplay.QuestSystem.Quest.Status.Enabled`
- `Gameplay.QuestSystem.Quest.Status.Disabled`
- `Gameplay.QuestSystem.Quest.Status.OnGoing`
- `Gameplay.QuestSystem.Quest.Status.Completed`

The primary condition mechanism is `UQuestWorldSubsystem::QuestGameplayTagsContainer` — game systems add tags here to represent world state, and `UQuestCondition` subclasses query it.
