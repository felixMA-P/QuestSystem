// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestWorldSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "ChainQuest.h"
#include "ChainQuestGraph.h"
#include "ChainQuestHandler.h"
#include "DataAssetChainQuests.h"
#include "EndQuestInfo.h"
#include "EndQuestResult.h"
#include "QuestInfo.h"
#include "QuestSystemPluginRuntime.h"
#include "QuestTagsManager.h"

void UQuestWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UQuestWorldSubsystem::Deinitialize()
{
	for (FChainQuestHandler* Handler : ChainQuests)    { delete Handler; }
	for (FChainQuestHandler* Handler : EndChainQuests) { delete Handler; }
	ChainQuests.Empty();
	EndChainQuests.Empty();

	Super::Deinitialize();
}

bool UQuestWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}


void UQuestWorldSubsystem::AddGameplayTags(const TArray<FGameplayTag>& GameplayTags)
{
	for (const FGameplayTag & GameplayTag : GameplayTags)
	{
		QuestGameplayTagsContainer.AddTag(GameplayTag);
	}

	CheckOnGoingQuestConditions();
}

void UQuestWorldSubsystem::InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer)
{
	if (!DataAssetInitializer)
	{
		UE_LOG(LogQuestSystem, Warning, TEXT("Quest Chain Quests Initialized, the parameter is not informed"));
		return;
	}
	
	for (const UChainQuest* ChainQuest : DataAssetInitializer->ChainQuests)
	{
		AddChainQuest(ChainQuest);
	}
}

void UQuestWorldSubsystem::CheckOnGoingQuestConditions()
{
	TArray<FChainQuestHandler*> AuxEndedChainQuests;

	for (FChainQuestHandler* OnGoingChainQuest : ChainQuests)
	{
		const UQuestInfoBase* NodeInfoBeforeCheck = OnGoingChainQuest->CurrentNode ? OnGoingChainQuest->CurrentNode->QuestInfo : nullptr;
		if (OnGoingChainQuest->CheckCurrentNodeConditions(GetWorld()))
		{
			const UEndQuestInfo* EndQuestInfo = Cast<UEndQuestInfo>(OnGoingChainQuest->CurrentNode->QuestInfo);
			if (!EndQuestInfo) continue;

			if (EndQuestInfo->NextChainQuest != nullptr)
			{
				AddChainQuest(EndQuestInfo->NextChainQuest);
			}

			if (EndQuestInfo->EndResult != nullptr)
			{
				EndQuestInfo->EndResult.GetDefaultObject()->ExecuteResult(GetWorld());
			}

			UE_LOG(LogQuestSystem, Log, TEXT("ChainQuest '%s' completed"), *OnGoingChainQuest->GetChainQuest()->Title.ToString());
			OnChainQuestCompleted.Broadcast(OnGoingChainQuest->GetChainQuest());
			AuxEndedChainQuests.Add(OnGoingChainQuest);
		}
		else if (OnGoingChainQuest->CurrentNode && OnGoingChainQuest->CurrentNode->QuestInfo != NodeInfoBeforeCheck)
		{
			OnQuestAdvanced.Broadcast(OnGoingChainQuest->CurrentNode->QuestInfo);
		}
	}

	for (FChainQuestHandler* ChainQuest : AuxEndedChainQuests)
	{
		EndChainQuests.Add(ChainQuest);
		ChainQuests.Remove(ChainQuest);
	}
}

void UQuestWorldSubsystem::AddChainQuest(const UChainQuest* ToAddChainQuest)
{
	if (!ToAddChainQuest)
		return;

	if (FindChainQuest(ToAddChainQuest))
		return;

	if (ToAddChainQuest->StartCondition == nullptr)
	{
		UE_LOG(LogQuestSystem, Log, TEXT("ChainQuest '%s' started"), *ToAddChainQuest->Title.ToString());
		ChainQuests.Add(ToAddChainQuest->GetHandler());
		ChainQuests.Last()->CheckCurrentEndDay(CurrentDay);
		OnChainQuestStarted.Broadcast(ToAddChainQuest);
		return;
	}

	if (ToAddChainQuest->StartCondition.GetDefaultObject()->CheckCondition(GetWorld()))
	{
		UE_LOG(LogQuestSystem, Log, TEXT("ChainQuest '%s' started (condition met)"), *ToAddChainQuest->Title.ToString());
		ChainQuests.Add(ToAddChainQuest->GetHandler());
		ChainQuests.Last()->CheckCurrentEndDay(CurrentDay);
		OnChainQuestStarted.Broadcast(ToAddChainQuest);
	}
	
}

bool UQuestWorldSubsystem::FindChainQuest(const UChainQuest* InChainQuest)
{
	return ChainQuests.FindByPredicate([InChainQuest](const FChainQuestHandler* InHandler)
	{
		return InHandler->ChainQuest == InChainQuest;
	}) != nullptr;
}

void UQuestWorldSubsystem::AddChainQuestToCalendar(const UChainQuest* ChainQuest, const int Day)
{
	CalendarChainQuests.Add(ChainQuest, Day);
}

void UQuestWorldSubsystem::CheckCalendar()
{
	TArray<const UChainQuest*> ToActivate;
	for (const TPair<const UChainQuest*, int>& CalendarPair : CalendarChainQuests)
	{
		if (CalendarPair.Value == CurrentDay)
		{
			ToActivate.Add(CalendarPair.Key);
		}
	}
	for (const UChainQuest* Quest : ToActivate)
	{
		AddChainQuest(Quest);
		CalendarChainQuests.Remove(Quest);
	}
}

void UQuestWorldSubsystem::CheckCalendarOnGoingQuests()
{
	TArray<FChainQuestHandler*> AuxEndedChainQuests;
	
	for (FChainQuestHandler * OnGoingChainQuest : ChainQuests)
	{
		if (OnGoingChainQuest->ChainQuest->bHasCalendarDates && OnGoingChainQuest->CheckCurrentEndDay(CurrentDay))
		{
			const UEndQuestInfo* EndQuestInfo = Cast<UEndQuestInfo>(OnGoingChainQuest->CurrentNode->QuestInfo);
			if (EndQuestInfo && EndQuestInfo->NextChainQuest != nullptr)
			{
				AddChainQuest(EndQuestInfo->NextChainQuest);
			}

			AuxEndedChainQuests.Add(OnGoingChainQuest);
		}
	}

	for (FChainQuestHandler* ChainQuest : AuxEndedChainQuests)
	{
		EndChainQuests.Add(ChainQuest);
		ChainQuests.Remove(ChainQuest);
	}
	
}

void UQuestWorldSubsystem::EndOfDay()
{
	CurrentDay++;
	CheckCalendar();
	CheckOnGoingQuestConditions();
	CheckCalendarOnGoingQuests();
}

void UQuestWorldSubsystem::GetAllChainQuestsInfo(FChainsQuestsInfo& OutInfo)
{
	for (const FChainQuestHandler* Element : ChainQuests)
	{
		OutInfo.ChainQuests.Add(Element->ChainQuest);
	}
	
	for (const FChainQuestHandler* Element : EndChainQuests)
	{
		OutInfo.EndChainQuests.Add(Element->ChainQuest);
	}
}

void UQuestWorldSubsystem::GetChainQuestQuestsInfo(const UChainQuest* ChainQuest, FQuestsInfo& OutInfo)
{
	if (!ChainQuest) return;
	
	FChainQuestHandler** HandlerPtr = ChainQuests.FindByPredicate([ChainQuest](FChainQuestHandler* ChainQuestHandler)
 	{
 		 return ChainQuestHandler->ChainQuest == ChainQuest;
	});
	
 	if (HandlerPtr)
	{
 		OutInfo.QuestsInfo = (*HandlerPtr)->QuestsInfos;
 	}
}

void UQuestWorldSubsystem::GetEndedChainQuestQuestsInfo(const UChainQuest* ChainQuest, FQuestsInfo& OutInfo)
{
	if (!ChainQuest) return;
	
	FChainQuestHandler** HandlerPtr = EndChainQuests.FindByPredicate([ChainQuest](FChainQuestHandler* ChainQuestHandler)
	{
		  return ChainQuestHandler->ChainQuest == ChainQuest;
	});
	
	if (HandlerPtr)
	{
		OutInfo.QuestsInfo = (*HandlerPtr)->QuestsInfos;
	}
}


