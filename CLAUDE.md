# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Unreal Engine 5.5 project hosting three gameplay plugins: a chain-quest system, a branching dialog system, and an NPC manager. The base `Source/QuestSystem` module is an empty primary game module (it only links `QuestSystemPluginRuntime`, plus the editor module in editor builds). All meaningful code lives under `Plugins/`. `Content/` holds example levels, quests and UI that exercise the plugins.

QuestSystemPlugin and DialogPlugin are published on Fab, and NPCManagerSystem is prepared to the same standard. Every source file in all three carries `// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.`, and each module has a `PlatformAllowList` in its `.uplugin`.

## Build

```
# Editor build (engine at A:\UE\Ins\UE_5.5):
A:\UE\Ins\UE_5.5\Engine\Build\BatchFiles\Build.bat QuestSystemEditor Win64 Development "A:\UE\Projects\QuestSystem\QuestSystem.uproject"

# Package a plugin standalone (see ComandsPackagePlugins.txt):
A:\UE\Ins\UE_5.5\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="A:\UE\Projects\QuestSystem\Plugins\<Plugin>\<Plugin>.uplugin" -Package="A:\UE\Packaged\<Plugin>"
```

There are no automated tests. Verify changes by building and exercising the example content in the editor.

## Repository Layout

| Plugin | Modules | Purpose |
|---|---|---|
| `QuestSystemPlugin` | `QuestSystemPluginRuntime` (Runtime), `QuestSystemPlugin` (Editor) | `UChainQuest` asset, quest execution subsystem, visual graph editor |
| `DialogPlugin` | `DialogPluginRuntime` (Runtime, uses UMG), `DialogPlugin` (Editor) | `UDialog` asset, single-active-dialog subsystem, widget base, graph editor |
| `NPCManagerSystem` | `NPCManagerSystem` (Runtime) | Tag-keyed NPC registry, per-NPC action queues |

The plugins do not depend on each other. `Docs/` has per-plugin reference docs (`QuestSystemPlugin.md`, `DialogPlugin.md`, `NPCManagerSystem.md`).

**Stale material to ignore:** `Docs/EventBusSystem.md`, the EventBus sections of `Docs/README.md`, and `Plugins/EventBus/` (untracked build output only, no source) belong to an event-bus plugin that does not exist on this branch.

## Shared Graph-Asset Pattern (Quest and Dialog)

DialogPlugin was built as a 1:1 mirror of QuestSystemPlugin. When changing one, check whether the counterpart needs the same change.

| Role | Quest | Dialog |
|---|---|---|
| Asset | `UChainQuest` | `UDialog` |
| Runtime graph | `UChainQuestGraph` → `UQuestRuntimeNode` → `UQuestRuntimePin` | `UDialogGraph` → `UDialogRuntimeNode` → `UDialogRuntimePin` |
| Node payload base | `UQuestInfoBase` | `UDialogInfoBase` |
| Regular node | `UQuestInfo` (`OutPuts`: `TArray<FQuestOutput>`) | `UDialogLineInfo` (`Outputs`: `TArray<FDialogOutput>`) |
| End node | `UEndQuestInfo` (`EndEvent`, `NextChainQuest`) | `UDialogEndInfo` (`EndEvent`, `NextDialog`) |
| Condition | `UQuestCondition::CheckCondition(UWorld*)` / BP `CheckConditionEvent` | `UDialogCondition`, same shape |
| Event | `UQuestEvent::Execute(UWorld*)` / BP `ExecuteEvent` | `UDialogEvent::ExecuteEvent(UObject*)` / BP `ExecuteEventBP` |
| Handler | `FChainQuestHandler` (plain C++, `new`/`delete`) | `UDialogHandler` (UObject, `NewObject`) |
| Subsystem | `UQuestWorldSubsystem` | `UDialogWorldSubsystem` |
| BP accessor | `USystemFunctionHelper::GetQuestSystem` | `UDialogFunctionHelper::GetDialogSystem` |
| Native tags | `FQuestTagsManager` | `FDialogTagsManager` |
| Node type enum | `EQuestNodeType` | `EDialogNodeType` (both: Start, Quest/Dialog, End, Knot, Unknown) |

Differences that matter:
- `FQuestOutput::Event` is `TSubclassOf<UQuestEvent>` and runs on the **CDO**. `FDialogOutput::Event` is an `Instanced` `UDialogEvent*` subobject (`UDialogEvent` is `EditInlineNew`) and runs on that **instance**. End-node `EndEvent`s are `TSubclassOf` and CDO-executed on both sides, as are all conditions.
- Output index `i` in `OutPuts`/`Outputs` corresponds to runtime `OutputPins[i]`. Keep the array and the pins in sync. The editor node's `SyncPinsWithOutputs` does this.
- Dialog has no calendar, no simple-tag conditions, and no data-asset loader (`UDialogDataAsset` was deleted).
- Runtime pins carry editor-only visual data: `ReroutePoints` (knot positions) and `Color`. Input pins mirror the colour of whatever is connected to them (`SyncInputPinColors`).

## Quest Runtime (`QuestSystemPluginRuntime`)

`UChainQuest` fields: `Title`, `bHasCalendarDates`, `StartCondition` (`TSubclassOf<UQuestCondition>`, unset means always start), `bUseSimpleConditions`, `StartConditionTag`, `ChainQuestGraph`.

**Simple conditions.** When `bUseSimpleConditions` is true, the Blueprint condition classes are ignored. `StartConditionTag` and each `FQuestOutput::ConditionTag` pass when that tag is present (`HasTagExact`) in `UQuestWorldSubsystem::QuestGameplayTagsContainer`. Outputs with an unset condition or tag are skipped, never matched.

`UQuestWorldSubsystem` (`UWorldSubsystem`) owns all state:
- `ChainQuests` / `EndChainQuests`: `TArray<FChainQuestHandler*>`, raw-owned and deleted in `Deinitialize`.
- `QuestGameplayTagsContainer`: world state. `AddGameplayTags()` adds tags **and immediately calls `CheckOnGoingQuestConditions()`**.
- `CalendarChainQuests` (`TMap<const UChainQuest*, int>`) and `CurrentDay` for scheduled starts.
- Delegates: `OnChainQuestStarted`, `OnQuestAdvanced`, `OnChainQuestCompleted`.
- Entry points: `InitializeChainQuests(UDataAssetChainQuests*)`, `AddChainQuest` (no-op if already active, gated on the start condition), `AddChainQuestToCalendar`, `EndOfDay`, and the `Get*Info` query functions.

**Advancement** (`FChainQuestHandler::CheckCurrentNodeConditions`): the handler starts on the node wired to the Start node's first output. For the current `UQuestInfo`, it walks `OutPuts` in array order. The first passing output fires its `Event`, moves to the connected node, appends to `QuestsInfos`, and recurses (depth-capped at 64). The function returns true on reaching an End node. The subsystem then queues `NextChainQuest`, runs `EndEvent`, broadcasts `OnChainQuestCompleted`, and moves the handler to `EndChainQuests`.

**Calendar** (`EndOfDay`): `++CurrentDay` → `CheckCalendar()` (starts quests scheduled for today) → `CheckOnGoingQuestConditions()` → `CheckCalendarOnGoingQuests()`. When a node's `DayToComplete > 0` and `CurrentDay >= DayToComplete`, it auto-advances via output pin 0. This path does not fire output events or `EndEvent`, and does not broadcast.

Native tags, registered at module startup: `Gameplay.QuestSystem.Quest.Status.{Enabled, Disabled, OnGoing, Completed}`. Log category: `LogQuestSystem`.

## Dialog Runtime (`DialogPluginRuntime`)

`UDialogWorldSubsystem` holds a single `ActiveDialog` (`UDialogHandler*`).
- `StartDialog(UDialog*)` checks `StartCondition` (CDO), creates the handler, and broadcasts `OnDialogStarted` and then `OnDialogLineChanged`.
- `SelectDialogResponse(Index)` fires that output's instanced event and follows `OutputPins[Index]`. When it reaches an End node, it clears the active dialog, broadcasts `OnDialogEnded`, runs `EndEvent`, and then auto-starts `NextDialog`.
- `UDialogLineInfo` adds `Speaker` (`FGameplayTag`), `DialogResume`, `DialogText`, and `GraphPreviewWrapLength` (editor title wrapping only).
- `DialogGameplayTagsContainer` / `AddGameplayTags` exist for conditions to query. Unlike the quest subsystem, adding tags does not re-evaluate anything.
- `UDialogWidget` (Abstract `UUserWidget`) auto-binds to the subsystem in `NativeConstruct`. Blueprint subclasses implement `OnLineChanged` and `OnDialogEnded` and call `SelectResponse(Index)`.

Native tags: `Gameplay.DialogSystem.Dialog.Status.{Active, Completed, Locked}`. The plugin logs to `LogTemp`.

## Graph Editors (`QuestSystemPlugin` / `DialogPlugin` editor modules)

The two editors are structurally identical, with `ChainQuest*` / `Quest*` names on one side and `Dialog*` names on the other:
- `F*AssetAction` (`FAssetTypeActions_Base`) opens `F*AssetEditorApp` (`FWorkflowCentricApplication` + `FEditorUndoClient`), which uses an `F*AssetAppMode` with a primary graph tab factory and a properties tab factory. `U*AssetCustomFactory` creates new assets.
- The editor edits a transient `UEdGraph` (`WorkingGraph`). `UpdateEditorGraphFromWorkingAsset()` builds it from the runtime graph on open. `UpdateWorkingAssetFromGraph()` rebuilds the runtime graph **on every graph change** (`OnGraphChanged`), on close, and on save.
- Knot (reroute) nodes exist only in the editor. On save, `ResolveRealQuestPin` walks through them, so runtime pins link node-to-node and the knot positions are stored in `ReroutePoints`. On load, knots are recreated from those points. Knot pins use the fixed subcategory `QuestKnotPin` / `DialogKnotPin`.
- Node classes live in `Public/Schemas/`: `U*GraphNodeBase` (the virtual interface: `CreateCustomPin`, `InitNodeInfo`, `GetQuestInfoBase`, `GetQuestNodeType`, `OnPropertiesChanged`, branch-colour helpers), and the Start (red, undeletable), regular (green, dynamic outputs, per-pin colour picker), End (blue) and Knot nodes.
- Schema (`UChainQuestGraphSchema` / `UDialogGraphSchema`): connections must go output→input. A new connection breaks existing links (`CONNECT_RESPONSE_BREAK_OTHERS_AB`). Double-clicking a wire inserts a knot.
- Undo/redo: node creation uses `RF_Transactional`, and mutations are wrapped in `FScopedTransaction` + `Modify()`. `PostUndo` / `PostRedo` resync. Keep this pattern for any new editor operation. Copy, cut, paste, duplicate and delete are implemented on the app.
- Editor icons live in each plugin's `Resources/`, loaded through a style set whose content root is `Plugin->GetBaseDir()/Resources`. Do not put loose non-asset files in `Content/` (Fab rejects it).
- UE 5.8 made `FApplicationMode::RegisterTabFactories` private. `DialogAssetAppMode.cpp` guards it with `UE_VERSION_OLDER_THAN(5, 8, 0)`; `ChainQuestAssetAppMode.cpp` does not yet.

## NPC Manager (`NPCManagerSystem`)

`UNpcManagerSubsystem` (`UWorldSubsystem`):
- **Registry.** `TagNPCsMap` maps `FGameplayTag` → `AActor`, one actor per tag. `AddNPC` is idempotent and refuses a tag already held by another live actor. Actors with a `UNPCSystemComponent` register at `OnActorsInitialized` (placed actors, before any BeginPlay) and from the component's `BeginPlay`, and unregister from its `EndPlay` via `RemoveNPCActor`. `SetTag` re-registers under a new tag. Other API: `RemoveNPC` (also cancels the NPC's actions), `GetNPC` (rejects actors mid-destruction), `MoveNPC`.
- **NPC info.** `UNPCManagerSettings` (`UDeveloperSettings`, Project Settings → Plugins → NPC Manager System) holds a soft `NPCDataTable` of `FNPCInfoRow` (Tag, Mesh3D, Image2D, Name, Description). It defaults to the plugin's `/NPCManagerSystem/NPCManagerSystem/Data/DT_NPCInfo`, is loaded in `Initialize`, and the module adds it to every cook via `FGameDelegates::ModifyCookDelegate` (nothing hard-references it). Rows are matched on their `Tag` **field**, not the row name, and `AddNPC` caches the matched row on the component.

**Actions** (command pattern):
- **Instances, not CDOs.** `UNPCAction` (`Abstract`, `EditInlineNew`, `Blueprintable`) is instanced per invocation, unlike quest/dialog conditions, and holds per-run state (`TargetNPC`, `TargetTag`).
- **Authored values.** An action is never run by class. `UNPCActionDataAsset` pairs `ActionClass` with `Defaults`, an `Instanced` object of that class that holds the authored values. `PostEditChangeProperty` rebuilds `Defaults` when `ActionClass` changes, and `InstantiateAction` duplicates `Defaults` for each run.
- **Queueing.** Each NPC has a FIFO queue (`ActionQueues`), and one action runs at a time. Queue work with `RunActionOnNPC` / `RunActionOnActor`, their `RunSoft*` variants (synchronous load of a `TSoftObjectPtr`), or `CreateNPCAction` + `QueueAction`. Other controls: `CancelNPCActions`, `GetRunningAction`, `GetQueuedActionCount`.
- **Finishing.** Actions are not ticked. `BeginAction` starts the work (a Delay, timer or async node) and must eventually call `FinishAction()`, otherwise the NPC's queue stalls. `GetWorld()` is overridden so latent Blueprint nodes work. `AbortAction` runs on cancel or world teardown.
- **Re-entrancy.** `StartNextAction` uses a `bAdvancingQueue` guard and re-reads the map on every pass, because an action can finish synchronously inside `BeginAction` or queue more actions.
- **Assets.** Plugin content lives in `Plugins/NPCManagerSystem/Content/NPCManagerSystem/{Actions,Data}/` (Fab pack-folder layout). Move assets only through the editor, never on disk.

Log category: `LogNPCManager` (use `Log LogNPCManager Verbose` to trace queues). BP accessor: `UFunctionHelpers::GetNpcManager`.

## Conventions

- CDO-executed extension classes (conditions, quest events, end events) must stay stateless.
- Blueprint hooks follow one pattern: a C++ `virtual` function whose default implementation calls a `BlueprintImplementableEvent` (e.g. `CheckCondition` → `CheckConditionEvent`).
- Packaging and Fab-release details are in the user's memory (`project_fab_publishing.md`), not in this repo.
