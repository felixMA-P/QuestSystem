# QuestSystem — Plugin Documentation

Unreal Engine 5.5 project. All gameplay code lives in three plugins under `Plugins/`.

## Plugins

| Plugin | Module(s) | Purpose |
|---|---|---|
| [QuestSystemPlugin](QuestSystemPlugin.md) | Runtime + Editor | Chain-quest data model, runtime execution, custom asset editor |
| [NPCManageSystem](NPCManageSystem.md) | Runtime | Tag-keyed NPC and world-entity registry |
| [DialogPlugin](DialogPlugin.md) | Runtime + Editor | Single-active-dialog system with Blueprint widget base |

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

## Build

```
Engine\Build\BatchFiles\Build.bat QuestSystemEditor Win64 Development "A:\UE\Projects\QuestSystem\QuestSystem.uproject"
```
