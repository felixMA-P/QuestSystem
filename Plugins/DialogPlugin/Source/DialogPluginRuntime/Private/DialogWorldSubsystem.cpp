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
	if (FindDialog(ToAddDialog)) return;

	if (ToAddDialog->StartCondition == nullptr)
	{
		Dialogs.Add(ToAddDialog->GetHandler(this));
		OnDialogLineChanged.Broadcast(ToAddDialog, Dialogs.Last()->GetCurrentDialogLine());
		return;
	}

	if (ToAddDialog->StartCondition.GetDefaultObject()->CheckCondition(GetWorld()))
	{
		Dialogs.Add(ToAddDialog->GetHandler(this));
		OnDialogLineChanged.Broadcast(ToAddDialog, Dialogs.Last()->GetCurrentDialogLine());
	}
}

bool UDialogWorldSubsystem::FindDialog(const UDialog* InDialog)
{
	return Dialogs.FindByPredicate([InDialog](const UDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	}) != nullptr;
}

bool UDialogWorldSubsystem::SelectDialogResponse(const UDialog* InDialog, int32 ResponseIndex)
{
	UDialogHandler** HandlerPtr = Dialogs.FindByPredicate([InDialog](UDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	if (!HandlerPtr) return false;

	UDialogHandler* Handler = *HandlerPtr;
	const bool bEnded = Handler->SelectResponse(ResponseIndex, GetWorld());

	if (bEnded)
	{
		const UDialogEndInfo* EndInfo = Cast<UDialogEndInfo>(Handler->CurrentNode->DialogInfo);

		EndDialogs.Add(Handler);
		Dialogs.Remove(Handler);

		// Broadcast after the handler is moved so GetCurrentDialogLine returns nullptr (correct) not a stale cast.
		OnDialogEnded.Broadcast(InDialog);

		if (EndInfo && EndInfo->EndResult != nullptr)
		{
			EndInfo->EndResult.GetDefaultObject()->ExecuteResult(GetWorld());
		}

		// Starting the next dialog fires OnDialogLineChanged for its first line.
		if (EndInfo && EndInfo->NextDialog != nullptr)
		{
			AddDialog(EndInfo->NextDialog);
		}
	}
	else
	{
		OnDialogLineChanged.Broadcast(InDialog, Handler->GetCurrentDialogLine());
	}

	return bEnded;
}

UDialogLineInfo* UDialogWorldSubsystem::GetCurrentDialogLine(const UDialog* InDialog)
{
	UDialogHandler** HandlerPtr = Dialogs.FindByPredicate([InDialog](UDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	return HandlerPtr ? (*HandlerPtr)->GetCurrentDialogLine() : nullptr;
}

void UDialogWorldSubsystem::GetAllDialogsInfo(FDialogsInfo& OutInfo)
{
	for (const UDialogHandler* Element : Dialogs)
	{
		OutInfo.Dialogs.Add(Element->Dialog);
	}

	for (const UDialogHandler* Element : EndDialogs)
	{
		OutInfo.EndDialogs.Add(Element->Dialog);
	}
}

void UDialogWorldSubsystem::GetDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo)
{
	if (!InDialog) return;

	UDialogHandler** HandlerPtr = Dialogs.FindByPredicate([InDialog](UDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	if (HandlerPtr)
	{
		OutInfo.DialogInfos = TArray<const UDialogInfoBase*>((*HandlerPtr)->DialogInfos);
	}
}

void UDialogWorldSubsystem::GetEndedDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo)
{
	if (!InDialog) return;

	UDialogHandler** HandlerPtr = EndDialogs.FindByPredicate([InDialog](UDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	if (HandlerPtr)
	{
		OutInfo.DialogInfos = TArray<const UDialogInfoBase*>((*HandlerPtr)->DialogInfos);
	}
}
