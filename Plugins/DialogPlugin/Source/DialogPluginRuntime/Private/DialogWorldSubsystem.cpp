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

	CheckOnGoingDialogConditions();
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

void UDialogWorldSubsystem::CheckOnGoingDialogConditions()
{
	TArray<FDialogHandler*> AuxEndedDialogs;

	for (FDialogHandler* OnGoingDialog : Dialogs)
	{
		if (OnGoingDialog->CheckCurrentNodeConditions(GetWorld()))
		{
			const UDialogEndInfo* EndInfo = Cast<UDialogEndInfo>(OnGoingDialog->CurrentNode->DialogInfo);

			if (EndInfo->NextDialog != nullptr)
			{
				AddDialog(EndInfo->NextDialog);
			}

			if (EndInfo->EndResult != nullptr)
			{
				EndInfo->EndResult.GetDefaultObject()->ExecuteResult(GetWorld());
			}

			AuxEndedDialogs.Add(OnGoingDialog);
		}
	}

	for (FDialogHandler* Dialog : AuxEndedDialogs)
	{
		EndDialogs.Add(Dialog);
		Dialogs.Remove(Dialog);
	}
}

void UDialogWorldSubsystem::AddDialog(const UDialog* ToAddDialog)
{
	if (!ToAddDialog) return;
	if (FindDialog(ToAddDialog)) return;

	if (ToAddDialog->StartCondition == nullptr)
	{
		Dialogs.Add(ToAddDialog->GetHandler());
		return;
	}

	if (ToAddDialog->StartCondition.GetDefaultObject()->CheckCondition(GetWorld()))
	{
		Dialogs.Add(ToAddDialog->GetHandler());
	}
}

bool UDialogWorldSubsystem::FindDialog(const UDialog* InDialog)
{
	return Dialogs.FindByPredicate([InDialog](const FDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	}) != nullptr;
}

void UDialogWorldSubsystem::GetAllDialogsInfo(FDialogsInfo& OutInfo)
{
	for (const FDialogHandler* Element : Dialogs)
	{
		OutInfo.Dialogs.Add(Element->Dialog);
	}

	for (const FDialogHandler* Element : EndDialogs)
	{
		OutInfo.EndDialogs.Add(Element->Dialog);
	}
}

void UDialogWorldSubsystem::GetDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo)
{
	if (!InDialog) return;

	FDialogHandler** HandlerPtr = Dialogs.FindByPredicate([InDialog](FDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	if (HandlerPtr)
	{
		OutInfo.DialogInfos = (*HandlerPtr)->DialogInfos;
	}
}

void UDialogWorldSubsystem::GetEndedDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo)
{
	if (!InDialog) return;

	FDialogHandler** HandlerPtr = EndDialogs.FindByPredicate([InDialog](FDialogHandler* Handler)
	{
		return Handler->Dialog == InDialog;
	});

	if (HandlerPtr)
	{
		OutInfo.DialogInfos = (*HandlerPtr)->DialogInfos;
	}
}
