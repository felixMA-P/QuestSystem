# DialogPlugin

Single-active-dialog system with a visual graph editor. Mirrors the QuestSystemPlugin architecture. Split into two modules:

- **DialogPluginRuntime** — All gameplay-facing classes. Depends on `GameplayTags`.
- **DialogPlugin** — Editor-only module. Custom asset editor for `UDialog` assets.

---

## Data Model

```
UDialog                                     ← One dialog tree asset
├── Title
├── StartCondition                          ← TSubclassOf<UDialogCondition>; nil = always starts
└── UDialogGraph
    └── UDialogRuntimeNode[]
        ├── UDialogLineInfo                 ← A spoken line with player responses
        │   ├── Speaker                     ← FGameplayTag identifying the speaker
        │   ├── DialogText                  ← The line to display
        │   └── Outputs[]                   ← Array of FDialogOutput (response text + event)
        └── UDialogEndInfo                  ← Terminal node
            ├── Title
            ├── EndEvent                    ← TSubclassOf<UDialogEvent>
            └── NextDialog                  ← Optional UDialog* to chain into
```

### UDialog

The top-level dialog asset. Create one per conversation tree.

| Property | Type | Description |
|---|---|---|
| `Title` | `FText` | Human-readable dialog name |
| `StartCondition` | `TSubclassOf<UDialogCondition>` | Optional gate — dialog only starts if this condition passes |
| `DialogGraph` | `UDialogGraph*` | Runtime graph; populated by the editor on save |

### UDialogLineInfo

A single spoken line in the conversation.

| Property | Type | Description |
|---|---|---|
| `Speaker` | `FGameplayTag` | Identifies the speaker (look up the actor via `UNpcManagerSubsystem`) |
| `DialogText` | `FText` | The line of dialog |
| `Outputs` | `TArray<FDialogOutput>` | Player response choices |

### FDialogOutput

One player-selectable response.

| Field | Type | Description |
|---|---|---|
| `ResponseText` | `FText` | Button label shown to the player |
| `Event` | `UDialogEvent*` (Instanced) | Optional event executed when the player picks this response |

### UDialogEndInfo

Terminal node that closes the dialog.

| Property | Type | Description |
|---|---|---|
| `Title` | `FString` | Label for this ending |
| `EndEvent` | `TSubclassOf<UDialogEvent>` | Event class executed when this node is reached |
| `NextDialog` | `UDialog*` | Optional dialog to start immediately after this one ends |

---

## Runtime Execution

### UDialogWorldSubsystem

`UWorldSubsystem` that manages the single active dialog. Only one dialog can be active at a time.

#### Delegates

Bind these in your UI Blueprint or widget:

| Delegate | Signature | Fires when |
|---|---|---|
| `OnDialogStarted` | `()` | A dialog is loaded and the first line is ready |
| `OnDialogLineChanged` | `(UDialogLineInfo*)` | The active line changes — start or after a response |
| `OnDialogEnded` | `()` | The dialog reaches an end node |

#### Functions

| Function | Description |
|---|---|
| `StartDialog(Dialog)` | Starts the given `UDialog` asset (checks `StartCondition` first) |
| `SelectDialogResponse(ResponseIndex)` | Called by UI when player picks a response; returns `true` if dialog ended |
| `GetCurrentDialogLine()` | Returns the current `UDialogLineInfo*` for display |
| `AddGameplayTags(Tags)` | Adds tags to `DialogGameplayTagsContainer` for use in conditions |

#### Key Properties

| Property | Type | Description |
|---|---|---|
| `DialogGameplayTagsContainer` | `FGameplayTagContainer` | World-state tags queried by `UDialogCondition` subclasses |

---

## Extension Points

### UDialogCondition

Gate a dialog behind arbitrary logic. Subclass in Blueprint or C++.

```cpp
// C++ override
virtual bool CheckCondition(UWorld* World);

// Blueprint: implement CheckConditionEvent
UFUNCTION(BlueprintImplementableEvent)
bool CheckConditionEvent(UWorld* World);
```

Called on the CDO. Query `UDialogWorldSubsystem::DialogGameplayTagsContainer` for world state.

### UDialogEvent

Runs logic when a player selects a response, or when a dialog end node is reached. Subclass in Blueprint or C++.

```cpp
// C++ override
virtual void ExecuteEvent(UObject* WorldContextObject);

// Blueprint: implement ExecuteEventBP
UFUNCTION(BlueprintImplementableEvent, meta = (WorldContext = "WorldContextObject"))
void ExecuteEventBP(UObject* WorldContextObject);
```

Marked `EditInlineNew` — can be created inline on a `FDialogOutput` in the asset editor.

---

## UDialogWidget — Blueprint UI Base

Inherit from `UDialogWidget` in a Widget Blueprint to create your dialog HUD.

The widget auto-binds to `UDialogWorldSubsystem` in `NativeConstruct` — no manual setup needed.

### Implementable Events

| Event | When to use |
|---|---|
| `OnLineChanged(LineInfo)` | Refresh speaker name, dialog text, and response buttons |
| `OnDialogEnded()` | Hide or animate out the dialog UI |

### Callable Functions

| Function | Description |
|---|---|
| `SelectResponse(ResponseIndex)` | Wire each response button's OnClicked to this; pass the button's index |
| `GetDialogSubsystem()` | Returns `UDialogWorldSubsystem*` for direct subsystem access |

### Typical Widget Setup

1. Create a Widget Blueprint inheriting `UDialogWidget`.
2. Implement `OnLineChanged`: populate speaker label, dialog text label, and dynamically create response buttons from `LineInfo.Outputs`.
3. Implement `OnDialogEnded`: play a close animation or remove the widget.
4. Bind each response button's `OnClicked` to `SelectResponse(index)`.

---

## Custom Asset Editor (Editor Module)

`FDialogAssetEditorApp` provides the visual graph editor for `UDialog` assets. Open it by double-clicking a `UDialog` asset in the Content Browser.

Mirrors the quest editor in structure:

| Node | Title Color | Wraps |
|---|---|---|
| `UDialogStartGraphNode` | Red | Entry point |
| `UDialogGraphNode` | — | `UDialogLineInfo` |
| `UDialogEndGraphNode` | Blue | `UDialogEndInfo` |

The schema (`UDialogGraphSchema`) enforces output→input connections only and breaks existing connections on reconnect.
