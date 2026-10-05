#include "NPCActionDataAsset.h"

#include "NPCAction.h"
#include "NPCManagerLog.h"

bool UNPCActionDataAsset::IsValidAction() const
{
	return ActionClass != nullptr && !ActionClass->HasAnyClassFlags(CLASS_Abstract);
}

UNPCAction* UNPCActionDataAsset::InstantiateAction(UObject* Outer) const
{
	if (!Outer)
	{
		return nullptr;
	}

	if (!ActionClass)
	{
		UE_LOG(LogNPCManager, Warning, TEXT("'%s' has no Action Class set"), *GetName());
		return nullptr;
	}

	// NewObject on an abstract class logs an engine error but still hands back an object,
	// which would then run the base class's empty BeginActionEvent and look like a no-op.
	if (ActionClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogNPCManager, Error,
			TEXT("'%s' has an abstract Action Class ('%s'). Pick a concrete subclass ")
			TEXT("— a Blueprint deriving from NPCAction, not NPCAction itself."),
			*GetName(), *ActionClass->GetName());
		return nullptr;
	}

	// Defaults is the authored copy; duplicating it carries every value across, including
	// Blueprint-declared ones, without this having to know what those properties are.
	if (Defaults && Defaults->GetClass() == ActionClass)
	{
		return DuplicateObject<UNPCAction>(Defaults, Outer);
	}

	// Reachable when the asset was built in C++ at runtime, or when ActionClass was changed
	// on a cooked/unsynced asset. The action still runs, just on its class defaults.
	if (Defaults)
	{
		UE_LOG(LogNPCManager, Warning,
			TEXT("'%s': Action Defaults is a '%s' but Action Class is '%s'. Re-save the asset to ")
			TEXT("rebuild it; running on class defaults meanwhile."),
			*GetName(), *Defaults->GetClass()->GetName(), *ActionClass->GetName());
	}

	return NewObject<UNPCAction>(Outer, ActionClass);
}

#if WITH_EDITOR

void UNPCActionDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName ChangedProperty = PropertyChangedEvent.GetPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(UNPCActionDataAsset, ActionClass))
	{
		SyncDefaultsToActionClass();
	}

	// Runs after the sync so the details panel rebuilds against the new Defaults object.
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UNPCActionDataAsset::SyncDefaultsToActionClass()
{
	const UClass* DesiredClass = IsValidAction() ? ActionClass.Get() : nullptr;

	if (Defaults && Defaults->GetClass() == DesiredClass)
	{
		return;
	}

	Modify();

	// RF_Transactional so editing the action's values, and this swap itself, are undoable.
	Defaults = DesiredClass
		? NewObject<UNPCAction>(this, ActionClass, NAME_None, RF_Transactional)
		: nullptr;
}

#endif
