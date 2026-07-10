# QuestSystemPlugin

Implements a graph-based chain-quest system. Split into two modules:

- **QuestSystemPluginRuntime** — All gameplay-facing classes. Depends on `GameplayTags`.
- **QuestSystemPlugin** — Editor-only module. Custom asset editor for `UChainQuest` assets.

---

## Data Model

```
UDataAssetChainQuests
└── UChainQuest[]               ← One quest chain (Blueprint/C++ subclassable UObject asset)
    ├── Title                   ← FText displayed in UI
    ├── bHasCalendarDates       ← Enables day-based expiry per quest node
    ├── bUseSimpleConditions    ← When true, all conditions in the chain use gameplay tags instead of UQuestCondition classes
    ├── StartCondition          ← TSubclassOf<UQuestCondition>; nil = always starts (used when bUseSimpleConditions is false)
    ├── StartConditionTag       ← FGameplayTag; unset = always starts (used when bUseSimpleConditions is true)
    └── UChainQuestGraph
        └── UQuestRuntimeNode[]
            ├── UQuestInfo      ← Regular quest node
            │   ├── Title
            │   ├── Description
            │   ├── DayToComplete   ← 0 = no expiry
            │   └── OutPuts         ← TMap<TSubclassOf<UQuestCondition>, FText>; each entry also carries a ConditionTag (FGameplayTag) used when bUseSimpleConditions is true
            └── UEndQuestInfo   ← Terminal node
                ├── Title
                ├── EndResult       ← TSubclassOf<UEndQuestResult>
                └── NextChainQuest  ← Optional UChainQuest* to auto-start
```

### UDataAssetChainQuests

Data asset that holds an array of `UChainQuest*` to load together.

```cpp
UPROPERTY(EditAnywhere, Category="Quest system")
TArray<UChainQuest*> ChainQuests;
```

### UChainQuest

The top-level quest-chain asset. Create one per quest arc. Subclass in Blueprint to add custom data.

| Property | Type | Description |
|---|---|---|
| `Title` | `FText` | Human-readable chain name |
| `bHasCalendarDates` | `bool` | When true, quest nodes use `DayToComplete` for expiry |
| `bUseSimpleConditions` | `bool` | When true, `StartCondition`/`FQuestOutput::Condition` are ignored in favor of `StartConditionTag`/`FQuestOutput::ConditionTag` for the whole chain |
| `StartCondition` | `TSubclassOf<UQuestCondition>` | Optional gate — chain only starts if this condition passes (used when `bUseSimpleConditions` is false) |
| `StartConditionTag` | `FGameplayTag` | Optional gate — chain only starts if this tag is present (exact match) in `QuestGameplayTagsContainer` (used when `bUseSimpleConditions` is true) |
| `ChainQuestGraph` | `UChainQuestGraph*` | Runtime graph; populated by the editor on save |

### UQuestInfo

A regular quest node. Holds the display data and the output conditions that determine which node comes next.

| Property | Type | Description |
|---|---|---|
| `Title` | `FText` | Quest title shown to the player |
| `Description` | `FText` | Quest description |
| `DayToComplete` | `int` | Days until automatic failure (0 = unlimited) |
| `OutPuts` | `TMap<TSubclassOf<UQuestCondition>, FText>` | Ordered map of conditions → display text; first passing condition advances the quest. Each entry also has a `ConditionTag` (`FGameplayTag`), used instead of the condition class when the owning chain's `bUseSimpleConditions` is true |

### UEndQuestInfo

A terminal node that closes the chain.

| Property | Type | Description |
|---|---|---|
| `Title` | `FText` | Label for this ending |
| `EndResult` | `TSubclassOf<UEndQuestResult>` | Result class executed (CDO) when this node is reached |
| `NextChainQuest` | `UChainQuest*` | Optional chain to start immediately after this one ends |

---

## Runtime Execution

### UQuestWorldSubsystem

`UWorldSubsystem` that owns all active quest state. Access via:

```cpp
UQuestWorldSubsystem* QS = USystemFunctionHelper::GetQuestSystem(GetWorld());
```

#### Delegates

| Delegate | Signature | Fires when |
|---|---|---|
| `OnChainQuestStarted` | `(const UChainQuest*)` | A chain quest is added and initialized |
| `OnQuestAdvanced` | `(const UQuestInfoBase*)` | The current node advances to a new node |
| `OnChainQuestCompleted` | `(const UChainQuest*)` | A chain quest reaches an end node |

#### Key Properties

| Property | Type | Description |
|---|---|---|
| `QuestGameplayTagsContainer` | `FGameplayTagContainer` | World-state tag store; `UQuestCondition` subclasses query this |

#### Functions

| Function | Description |
|---|---|
| `InitializeChainQuests(DataAsset)` | Loads and starts all chains in the data asset |
| `AddChainQuest(ChainQuest)` | Adds and initializes a single chain quest |
| `FindChainQuest(ChainQuest)` | Returns true if the chain is already active |
| `AddGameplayTags(Tags)` | Adds tags to `QuestGameplayTagsContainer` to signal world events |
| `CheckOnGoingQuestConditions()` | Evaluates all active quest conditions; call after state changes |
| `AddChainQuestToCalendar(ChainQuest, Day)` | Schedules a chain quest to start on a specific day |
| `CheckCalendar()` | Checks the calendar map and starts any due chain quests |
| `CheckCalendarOnGoingQuests()` | Expires active quests whose `DayToComplete` has passed |
| `EndOfDay()` | Advances the internal day counter; calls calendar checks automatically |
| `GetAllChainQuestsInfo(OutInfo)` | Returns both in-progress and completed chain quests |
| `GetChainQuestQuestsInfo(Chain, OutInfo)` | Returns quest nodes for an active chain |
| `GetEndedChainQuestQuestsInfo(Chain, OutInfo)` | Returns quest nodes for a completed chain |

#### Condition Evaluation Flow

1. `CheckOnGoingQuestConditions()` iterates all active `FChainQuestHandler` instances.
2. Each handler calls `CheckCurrentNodeConditions(World)`.
3. `UQuestInfo::OutPuts` keys are iterated in order; the first `UQuestCondition::CheckCondition()` that returns `true` advances `CurrentNode` along the matching output pin.
4. Advancement recurses until an `EndNode` or no condition fires.
5. On `EndNode`: `UEndQuestResult::ExecuteResult()` is called, then `NextChainQuest` is queued if set.

#### Calendar Flow

`EndOfDay()` → increments `CurrentDay` → `CheckCalendar()` → `CheckOnGoingQuestConditions()` → `CheckCalendarOnGoingQuests()`.

Quests with `DayToComplete > 0` automatically expire by taking the first output pin when the day limit is reached.

---

## Extension Points

### UQuestCondition

Subclass in Blueprint or C++ to define custom quest advancement conditions.

```cpp
// C++ override
virtual bool CheckCondition(UWorld* World);

// Blueprint: implement CheckConditionEvent
UFUNCTION(BlueprintImplementableEvent, Category = "Quest System|Condition")
bool CheckConditionEvent(UWorld* World);
```

For the common case of "does the player have tag X", a `UQuestCondition` subclass isn't required at all — enable `UChainQuest::bUseSimpleConditions` and author `StartConditionTag`/`FQuestOutput::ConditionTag` instead. These are checked directly against `UQuestWorldSubsystem::QuestGameplayTagsContainer` with an exact tag match, bypassing `UQuestCondition` entirely for that chain.

The default C++ `CheckCondition` calls `CheckConditionEvent` and returns its result. Both are called on the CDO (class default object).

**Common pattern:** query `UQuestWorldSubsystem::QuestGameplayTagsContainer` inside your condition to check for tags that game systems have added via `AddGameplayTags()`.

### UEndQuestResult

Subclass to define logic that runs when a quest chain ends.

```cpp
// C++ override
virtual void ExecuteResult(UWorld* World);

// Blueprint: implement ExecuteResultEvent
UFUNCTION(BlueprintImplementableEvent, Category = "Quest System|Result")
void ExecuteResultEvent(UWorld* World);
```

Also called on the CDO.

---

## Gameplay Tags

Quest status tags are registered natively via `FQuestTagsManager` at plugin startup:

| Tag |
|---|
| `Gameplay.QuestSystem.Quest.Status.Enabled` |
| `Gameplay.QuestSystem.Quest.Status.Disabled` |
| `Gameplay.QuestSystem.Quest.Status.OnGoing` |
| `Gameplay.QuestSystem.Quest.Status.Completed` |

---

## Blueprint Access

```cpp
// Get subsystem from any Blueprint with World access
USystemFunctionHelper::GetQuestSystem(World)  →  UQuestWorldSubsystem*
```

---

## Custom Asset Editor (Editor Module)

`FChainQuestAssetEditorApp` is a `FWorkflowCentricApplication` that provides the visual graph editor for `UChainQuest` assets. Open it by double-clicking a `UChainQuest` asset in the Content Browser.

### How It Works

- The editor works on a `UEdGraph` (`WorkingGraph`) as the visual representation.
- On open, the runtime graph (`UChainQuestGraph`) is translated into editor nodes.
- On save, editor nodes are synced back to `UChainQuest::ChainQuestGraph`.

### Editor Node Types

| Class | Title Color | Deletable | Description |
|---|---|---|---|
| `UQuestStartGraphNode` | Red | No | Entry point; single output pin |
| `UQuestGraphNode` | Green | Yes | Wraps `UQuestInfo`; output pins sync from `OutPuts` map |
| `UQuestEndGraphNode` | Blue | Yes | Wraps `UEndQuestInfo`; terminal node |

### Graph Schema: `UChainQuestGraphSchema`

- Only output→input connections are allowed.
- Connecting an output pin that already has a connection breaks the old connection first.
- Right-click the graph to add Quest or End nodes.
