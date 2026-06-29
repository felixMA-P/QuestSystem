#include "DialogWorldSubsystem.h"

#include "Dialog.h"
#include "DialogDataAsset.h"
#include "DialogEndInfo.h"
#include "DialogGraph.h"
#include "DialogHandler.h"
#include "DialogResult.h"

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

void UDialogWorldSubsystem::InitializeDialogs(const UDialogDataAsset* DataAsset)
{
	if (!DataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialog System: InitializeDialogs called with null DataAsset"));
		return;
	}

	for (const UDialog* Dialog : DataAsset->Dialogs)
	{
		AddDialog(Dialog);
	}
}

void UDialogWorldSubsystem::AddDialog(const UDialog* ToAddDialog)
{
	if (!ToAddDialog) return;
	if (ActiveDialog && ActiveDialog->GetDialog() == ToAddDialog) return;

	if (ToAddDialog->StartCondition == nullptr)
	{
		ActiveDialog = ToAddDialog->GetHandler(this);
		OnDialogLineChanged.Broadcast(ActiveDialog->GetCurrentDialogLine());
		return;
	}

	if (ToAddDialog->StartCondition.GetDefaultObject()->CheckCondition(GetWorld()))
	{
		ActiveDialog = ToAddDialog->GetHandler(this);
		OnDialogLineChanged.Broadcast(ActiveDialog->GetCurrentDialogLine());
	}
}

bool UDialogWorldSubsystem::SelectDialogResponse(int32 ResponseIndex)
{
	if (!ActiveDialog) return false;

	const bool bEnded = ActiveDialog->SelectResponse(ResponseIndex, GetWorld());

	if (bEnded)
	{
		const UDialogEndInfo* EndInfo = Cast<UDialogEndInfo>(ActiveDialog->CurrentNode->DialogInfo);
		UDialog* NextDialog           = (EndInfo && EndInfo->NextDialog) ? EndInfo->NextDialog : nullptr;
		TSubclassOf<UDialogResult> EndResult = EndInfo ? EndInfo->EndResult : nullptr;

		ActiveDialog = nullptr;
		OnDialogEnded.Broadcast();

		if (EndResult)
			EndResult.GetDefaultObject()->ExecuteResult(GetWorld());

		if (NextDialog)
			AddDialog(NextDialog);
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
