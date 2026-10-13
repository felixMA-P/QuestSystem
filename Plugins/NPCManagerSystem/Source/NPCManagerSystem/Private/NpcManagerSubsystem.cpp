// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.


#include "NpcManagerSubsystem.h"
#include "EngineUtils.h"
#include "NPCAction.h"
#include "NPCActionDataAsset.h"
#include "NPCInfoRow.h"
#include "NPCManagerLog.h"
#include "NPCManagerSettings.h"
#include "NPCSystemComponent.h"

void UNpcManagerSubsystem::AddNPC(AActor* NPC)
{
	if (!NPC)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("AddNPC: null actor"));
		return;
	}

	UNPCSystemComponent* Comp = NPC->FindComponentByClass<UNPCSystemComponent>();

	if (!Comp)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("AddNPC: '%s' has no NPC System Component, so it has no tag to register under"),
			*NPC->GetActorNameOrLabel());
		return;
	}

	if (!Comp->Tag.IsValid())
	{
		UE_LOG(LogNPCManager, Error, TEXT("AddNPC: '%s' has no tag set on its NPC System Component"),
			*NPC->GetActorNameOrLabel());
		return;
	}

	// Registration runs from several places (world init, BeginPlay, manual calls), so
	// re-adding the same actor is expected and harmless.
	if (const TObjectPtr<AActor>* Existing = TagNPCsMap.Find(Comp->Tag))
	{
		if (*Existing == NPC)
		{
			return;
		}

		// A destroyed holder is just a stale entry; anything live means two NPCs share a tag,
		// and silently replacing it would leave GetNPC answering for whichever registered last.
		if (IsValid(*Existing))
		{
			UE_LOG(LogNPCManager, Error, TEXT("AddNPC: tag '%s' is already used by '%s'; '%s' was not registered. NPC tags must be unique."),
				*Comp->Tag.GetTagName().ToString(), *(*Existing)->GetActorNameOrLabel(), *NPC->GetActorNameOrLabel());
			return;
		}
	}

	// Rows are optional, so a missing one is not worth a warning here.
	const FNPCInfoRow* Row = FindNPCInfoRow(Comp->Tag);
	Comp->SetNPCData(Row ? *Row : FNPCInfoRow());

	TagNPCsMap.Add(Comp->Tag, NPC);

	UE_LOG(LogNPCManager, Verbose, TEXT("Registered '%s' as '%s'"),
		*NPC->GetActorNameOrLabel(), *Comp->Tag.GetTagName().ToString());
}

void UNpcManagerSubsystem::RemoveNPC(const FGameplayTag NameTag)
{
	// Nothing queued should outlive the NPC it targets.
	CancelNPCActions(NameTag);

	TagNPCsMap.Remove(NameTag);
}

void UNpcManagerSubsystem::RemoveNPCActor(AActor* NPC)
{
	if (!NPC)
	{
		return;
	}

	// Looked up by actor, not by its component's current tag: the tag may have changed
	// since registration, or the actor may never have won it.
	if (const FGameplayTag* Tag = TagNPCsMap.FindKey(NPC))
	{
		RemoveNPC(*Tag);
	}
}

AActor* UNpcManagerSubsystem::GetNPC(const FGameplayTag NameTag)
{
	const TObjectPtr<AActor>* NpcPtr = TagNPCsMap.Find(NameTag);

	// IsValid also rejects an actor that is mid-destruction but not yet collected.
	if (!NpcPtr || !IsValid(*NpcPtr))
	{
		UE_LOG(LogNPCManager, Warning, TEXT("No NPC found for tag: %s"), *NameTag.GetTagName().ToString());
		return nullptr;
	}

	return *NpcPtr;
}

const FNPCInfoRow* UNpcManagerSubsystem::FindNPCInfoRow(const FGameplayTag& Tag) const
{
	if (!NPCDataTable)
	{
		return nullptr;
	}

	const FNPCInfoRow* FoundRow = nullptr;
	NPCDataTable->ForeachRow<FNPCInfoRow>(TEXT("FindNPCInfoRow"), [&Tag, &FoundRow](const FName& RowName, const FNPCInfoRow& Row)
	{
		if (!FoundRow && Row.Tag == Tag)
		{
			FoundRow = &Row;
		}
	});

	return FoundRow;
}

bool UNpcManagerSubsystem::GetNPCInfo(const FGameplayTag& Tag, FNPCInfoRow& OutInfo)
{
	if (!NPCDataTable)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("GetNPCInfo: no NPC DataTable is loaded (Project Settings > Plugins > NPC Manager System)"));
		return false;
	}

	const FNPCInfoRow* FoundRow = FindNPCInfoRow(Tag);

	if (!FoundRow)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("GetNPCInfo: No row found for tag '%s' in DataTable '%s'"),
			*Tag.GetTagName().ToString(), *NPCDataTable->GetName());
		return false;
	}

	OutInfo = *FoundRow;
	return true;
}

void UNpcManagerSubsystem::MoveNPC(const FGameplayTag NameTag, const FVector& Location)
{
	// GetNPC logs when the tag is unknown.
	if (AActor* Npc = GetNPC(NameTag))
	{
		Npc->SetActorLocation(Location);
	}
}


UNPCAction* UNpcManagerSubsystem::CreateNPCAction(const UNPCActionDataAsset* ActionAsset)
{
	if (!ActionAsset)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("CreateNPCAction: no action data asset given"));
		return nullptr;
	}

	// The asset owns class validation and the copying of authored values; it logs its own reasons.
	UNPCAction* Action = ActionAsset->InstantiateAction(this);

	if (!Action)
	{
		return nullptr;
	}

	Action->OnCompletedInternal.BindUObject(this, &UNpcManagerSubsystem::HandleActionCompleted);

	return Action;
}

bool UNpcManagerSubsystem::QueueAction(const FGameplayTag NPCTag, UNPCAction* Action)
{
	if (!Action)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("QueueAction: null action for tag '%s'"),
			*NPCTag.GetTagName().ToString());
		return false;
	}

	if (Action->bRunning || Action->bFinished)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("QueueAction: '%s' has already been used; create a new instance"),
			*Action->GetName());
		return false;
	}

	// GetNPC logs when the tag is unknown.
	if (!GetNPC(NPCTag))
	{
		return false;
	}

	FNPCActionQueue& Queue = ActionQueues.FindOrAdd(NPCTag);
	Queue.Pending.Add(Action);

	const bool bIdle = (Queue.Running == nullptr);

	if (!bIdle)
	{
		// Not an error, but the usual reason an action "never starts": the one ahead of it
		// has not called FinishAction, so nothing behind it can begin.
		UE_LOG(LogNPCManager, Log,
			TEXT("QueueAction: '%s' is waiting on '%s', which is still running on '%s' (%d now queued)"),
			*Action->GetName(), *Queue.Running->GetName(),
			*NPCTag.GetTagName().ToString(), Queue.Pending.Num());
	}

	// Queue may be reallocated past this point; don't touch the reference again.
	if (bIdle)
	{
		StartNextAction(NPCTag);
	}

	return true;
}

UNPCAction* UNpcManagerSubsystem::RunActionOnNPC(const FGameplayTag NPCTag, const UNPCActionDataAsset* ActionAsset)
{
	UNPCAction* Action = CreateNPCAction(ActionAsset);

	if (!Action)
	{
		return nullptr;
	}

	return QueueAction(NPCTag, Action) ? Action : nullptr;
}

UNPCAction* UNpcManagerSubsystem::RunActionOnActor(AActor* NPC, const UNPCActionDataAsset* ActionAsset)
{
	if (!NPC)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunActionOnActor: null actor"));
		return nullptr;
	}

	const UNPCSystemComponent* Comp = NPC->GetComponentByClass<UNPCSystemComponent>();

	if (!Comp || !Comp->Tag.IsValid())
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunActionOnActor: '%s' has no NPC component with a valid tag"),
			*NPC->GetName());
		return nullptr;
	}

	return RunActionOnNPC(Comp->Tag, ActionAsset);
}

UNPCAction* UNpcManagerSubsystem::RunSoftActionOnNPC(const FGameplayTag NPCTag, TSoftObjectPtr<UNPCActionDataAsset> ActionAsset)
{
	// A null soft pointer and one whose asset failed to load are different mistakes; the
	// second is the one worth a message, since the reference looks fine in the editor.
	if (ActionAsset.IsNull())
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunSoftActionOnNPC: no action data asset given"));
		return nullptr;
	}

	const UNPCActionDataAsset* Loaded = ActionAsset.LoadSynchronous();

	if (!Loaded)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunSoftActionOnNPC: failed to load '%s'"),
			*ActionAsset.ToString());
		return nullptr;
	}

	return RunActionOnNPC(NPCTag, Loaded);
}

UNPCAction* UNpcManagerSubsystem::RunSoftActionOnActor(AActor* NPC, TSoftObjectPtr<UNPCActionDataAsset> ActionAsset)
{
	if (ActionAsset.IsNull())
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunSoftActionOnActor: no action data asset given"));
		return nullptr;
	}

	const UNPCActionDataAsset* Loaded = ActionAsset.LoadSynchronous();

	if (!Loaded)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("RunSoftActionOnActor: failed to load '%s'"),
			*ActionAsset.ToString());
		return nullptr;
	}

	return RunActionOnActor(NPC, Loaded);
}

void UNpcManagerSubsystem::StartNextAction(const FGameplayTag& NPCTag)
{
	// An action can finish inside BeginAction, or queue further actions. Both mutate
	// ActionQueues, so the map is re-read every pass and the guard keeps completions
	// from recursing back into here.
	TGuardValue<bool> AdvanceGuard(bAdvancingQueue, true);

	while (true)
	{
		FNPCActionQueue* Queue = ActionQueues.Find(NPCTag);

		if (!Queue || Queue->Running)
		{
			return;
		}

		if (Queue->Pending.Num() == 0)
		{
			ActionQueues.Remove(NPCTag);
			return;
		}

		UNPCAction* Next = Queue->Pending[0];
		Queue->Pending.RemoveAt(0);

		if (!Next)
		{
			continue;
		}

		// Re-resolved per action: the NPC may have been removed while this one waited.
		AActor* Target = GetNPC(NPCTag);

		if (!Target)
		{
			Next->Abort();
			ActionQueues.Remove(NPCTag);
			return;
		}

		Queue->Running = Next;

		UE_LOG(LogNPCManager, Verbose, TEXT("Starting action '%s' on '%s'"),
			*Next->GetName(), *NPCTag.GetTagName().ToString());

		Next->Start(Target, NPCTag);
	}
}

void UNpcManagerSubsystem::HandleActionCompleted(UNPCAction* Action)
{
	if (!Action)
	{
		return;
	}

	const FGameplayTag NPCTag = Action->TargetTag;
	FNPCActionQueue* Queue = ActionQueues.Find(NPCTag);

	if (!Queue || Queue->Running != Action)
	{
		return;
	}

	Queue->Running = nullptr;

	// Inside StartNextAction its loop picks this up; otherwise advance now.
	if (!bAdvancingQueue)
	{
		StartNextAction(NPCTag);
	}
}

void UNpcManagerSubsystem::CancelNPCActions(const FGameplayTag NPCTag)
{
	FNPCActionQueue Queue;

	// Detach the queue first so aborting can't re-enter it.
	if (!ActionQueues.RemoveAndCopyValue(NPCTag, Queue))
	{
		return;
	}

	for (UNPCAction* Pending : Queue.Pending)
	{
		if (Pending)
		{
			Pending->Abort();
		}
	}

	if (Queue.Running)
	{
		Queue.Running->Abort();
	}
}

UNPCAction* UNpcManagerSubsystem::GetRunningAction(const FGameplayTag NPCTag) const
{
	const FNPCActionQueue* Queue = ActionQueues.Find(NPCTag);

	return Queue ? Queue->Running.Get() : nullptr;
}

int32 UNpcManagerSubsystem::GetQueuedActionCount(const FGameplayTag NPCTag) const
{
	const FNPCActionQueue* Queue = ActionQueues.Find(NPCTag);

	return Queue ? Queue->Pending.Num() : 0;
}

void UNpcManagerSubsystem::InitNpcList(const FActorsInitializedParams& ActorsInitializedParams)
{
	// Runs before any BeginPlay, so NPCs placed in the level can be looked up from the start.
	// Actors spawned later register themselves from UNPCSystemComponent::BeginPlay.
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->FindComponentByClass<UNPCSystemComponent>())
		{
			AddNPC(*It);
		}
	}
}

void UNpcManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Loaded here rather than in the constructor: the CDO is created at module load, which can
	// precede the plugin's content mount, in which case the asset would not be found.
	const TSoftObjectPtr<UDataTable>& TablePath = GetDefault<UNPCManagerSettings>()->NPCDataTable;

	if (!TablePath.IsNull())
	{
		NPCDataTable = TablePath.LoadSynchronous();

		if (!NPCDataTable)
		{
			UE_LOG(LogNPCManager, Warning, TEXT("NPC Manager: failed to load the NPC DataTable at '%s'"), *TablePath.ToString());
		}
		else if (!NPCDataTable->GetRowStruct() || !NPCDataTable->GetRowStruct()->IsChildOf(FNPCInfoRow::StaticStruct()))
		{
			UE_LOG(LogNPCManager, Error, TEXT("NPC Manager: DataTable '%s' does not use the NPCInfoRow row structure; ignoring it"), *TablePath.ToString());
			NPCDataTable = nullptr;
		}
	}

	OnActorsInitializedDelegate = GetWorld()->OnActorsInitialized.Add(UWorld::FOnWorldInitializedActors::FDelegate::CreateUObject(this, &UNpcManagerSubsystem::InitNpcList));
}

void UNpcManagerSubsystem::Deinitialize()
{
	GetWorld()->OnActorsInitialized.Remove(OnActorsInitializedDelegate);

	// Abort anything still in flight; a pending Delay or timer must not fire into a dead world.
	TArray<FGameplayTag> QueuedTags;
	ActionQueues.GetKeys(QueuedTags);

	for (const FGameplayTag& Tag : QueuedTags)
	{
		CancelNPCActions(Tag);
	}

	ActionQueues.Empty();

	Super::Deinitialize();
}

bool UNpcManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
