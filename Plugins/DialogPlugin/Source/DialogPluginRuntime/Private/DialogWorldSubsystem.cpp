#include "DialogWorldSubsystem.h"

#include "Dialog.h"
#include "DialogEndInfo.h"
#include "DialogGraph.h"
#include "DialogHandler.h"
#include "DialogEvent.h"
#include "Engine/World.h"

void UDialogWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDialogWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UDialogWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UDialogWorldSubsystem::AddGameplayTags(const TArray<FGameplayTag>& GameplayTags)
{
	for (const FGameplayTag& Tag : GameplayTags)
	{
		DialogGameplayTagsContainer.AddTag(Tag);
	}
}

void UDialogWorldSubsystem::StartDialog(UDialog* Dialog)
{
	if (!Dialog)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialog System: StartDialog called with a null Dialog"));
		return;
	}

	StartDialogInternal(Dialog);
}

void UDialogWorldSubsystem::StartDialogInternal(UDialog* Dialog)
{
	if (ActiveDialog && ActiveDialog->GetDialog() == Dialog) return;

	if (Dialog->StartCondition && !Dialog->StartCondition.GetDefaultObject()->CheckCondition(GetWorld()))
		return;

	ActiveDialog = Dialog->GetHandler(this);
	OnDialogStarted.Broadcast();
	OnDialogLineChanged.Broadcast(ActiveDialog->GetCurrentDialogLine());
}

bool UDialogWorldSubsystem::SelectDialogResponse(int32 ResponseIndex)
{
	if (!ActiveDialog) return false;

	const bool bEnded = ActiveDialog->SelectResponse(ResponseIndex, GetWorld());

	if (bEnded)
	{
		const UDialogEndInfo* EndInfo = Cast<UDialogEndInfo>(ActiveDialog->CurrentNode->DialogInfo);
		UDialog* NextDialog           = (EndInfo && EndInfo->NextDialog) ? EndInfo->NextDialog : nullptr;
		TSubclassOf<UDialogEvent> EndResult = EndInfo ? EndInfo->EndEvent : nullptr;

		ActiveDialog = nullptr;
		OnDialogEnded.Broadcast();

		if (EndResult)
			EndResult.GetDefaultObject()->ExecuteEvent(GetWorld());

		if (NextDialog)
			StartDialogInternal(NextDialog);
	}
	else
	{
		OnDialogLineChanged.Broadcast(ActiveDialog->GetCurrentDialogLine());
	}

	return bEnded;
}

UDialogLineInfo* UDialogWorldSubsystem::GetCurrentDialogLine()
{
	return ActiveDialog ? ActiveDialog->GetCurrentDialogLine() : nullptr;
}
