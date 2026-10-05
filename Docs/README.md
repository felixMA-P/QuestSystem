# QuestSystem — Plugin Documentation

Unreal Engine 5.5 project. All gameplay code lives in four plugins under `Plugins/`.

## Plugins

| Plugin | Module(s) | Purpose |
|---|---|---|
| [QuestSystemPlugin](QuestSystemPlugin.md) | Runtime + Editor | Chain-quest data model, runtime execution, custom asset editor |
| [NPCManagerSystem](NPCManagerSystem.md) | Runtime | Tag-keyed NPC registry |
| [DialogPlugin](DialogPlugin.md) | Runtime + Editor | Single-active-dialog system with Blueprint widget base |
| [EventBusSystem](EventBusSystem.md) | Runtime + Bridges | Tag-driven event bus, rule matching, response execution, fired-ledger |

## Quick Start

### Running Quests

```cpp
// 1. Get the subsystem
UQuestWorldSubsystem* QS = USystemFunctionHelper::GetQuestSystem(GetWorld());

// 2. Load a chain-quest data asset and initialize
QS->InitializeChainQuests(MyDataAsset);

// 3. Signal world state via gameplay tags
QS->AddGameplayTags({ Tag_PlayerTalkedToNPC });

// 4. Evaluate conditions (call after any state change that might advance quests)
QS->CheckOnGoingQuestConditions();
```

### Running Dialogs

```cpp
// 1. Get the subsystem
UDialogWorldSubsystem* DS = GetWorld()->GetSubsystem<UDialogWorldSubsystem>();

// 2. Start a dialog (its optional StartCondition is checked first)
DS->StartDialog(MyDialogAsset);

// 3. The UI handles the rest via OnDialogLineChanged / SelectDialogResponse
```

### Posting Events

```cpp
// 1. Get the subsystem
UEventBusSubsystem* EB = UEventBusFunctionHelper::GetEventBus(GetWorld());

// 2. Tell it what the world looks like
EB->SetFact(Tag_PlayerHealth, 25.0);

// 3. Post what happened; matching rules run their responses and the ledger records the fire
FGameEvent Event(Tag_CombatKill);
Event.Instigator = Killer;
EB->PostEvent(Event);

// 4. Ask what already happened, at any time
const bool bEverKilled = EB->HasEventFired(Tag_CombatKill);
```

## Build

```
Engine\Build\BatchFiles\Build.bat QuestSystemEditor Win64 Development "A:\UE\Projects\QuestSystem\QuestSystem.uproject"
```
