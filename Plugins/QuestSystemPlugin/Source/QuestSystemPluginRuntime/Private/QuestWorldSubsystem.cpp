// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestWorldSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "ChainQuest.h"
#include "ChainQuestGraph.h"
#include "ChainQuestHandler.h"
#include "DataAssetChainQuests.h"
#include "EndQuestInfo.h"
#include "QuestInfo.h"
#include "QuestTagsManager.h"

void UQuestWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UQuestWorldSubsystem::Deinitialize()
{
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
}

void UQuestWorldSubsystem::InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer)
{
	if (!DataAssetInitializer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest Chain Quests Initialized, the parameter is not informed"));
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
		if (OnGoingChainQuest->CheckCurrentNodeConditions())
		{
			const UEndQuestInfo* EndQuestInfo = Cast<UEndQuestInfo>(OnGoingChainQuest->CurrentNode->QuestInfo);
			
			if (EndQuestInfo->NextChainQuest != nullptr)
			{
				AddChainQuest(EndQuestInfo->NextChainQuest);
			}
			
			if (EndQuestInfo->EndOutput != nullptr)
			{
				EndQuestInfo->EndOutput->GetDefaultObject<ACondition>()->CheckCondition();
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

void UQuestWorldSubsystem::AddChainQuest(const UChainQuest* ToAddChainQuest)
{
	if (!ToAddChainQuest)
		return;

	if (FindChainQuest(ToAddChainQuest))
		return;

	if (ToAddChainQuest->StartCondition == nullptr)
	{
		ChainQuests.Add(ToAddChainQuest->GetHandler());
		ChainQuests.Last()->CheckCurrentEndDay(CurrentDay);
		return;
	}
	
	if (ToAddChainQuest->StartCondition->GetDefaultObject<ACondition>()->CheckCondition())
	{
		ChainQuests.Add(ToAddChainQuest->GetHandler());
		ChainQuests.Last()->CheckCurrentEndDay(CurrentDay);
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
	for (const TPair<const UChainQuest*, int> & CalendarPair : CalendarChainQuests)
	{
		if (CalendarPair.Value == CurrentDay)
		{
			AddChainQuest(CalendarPair.Key);
			CalendarChainQuests.Remove(CalendarPair.Key);
		}
	}
}

void UQuestWorldSubsystem::CheckCalendarOnGoingQuests()
{
	TArray<FChainQuestHandler*> AuxEndedChainQuests;
	
	for (FChainQuestHandler * OnGoingChainQuest : ChainQuests)
	{
		if (OnGoingChainQuest->ChainQuest->bHasCalendarDates && OnGoingChainQuest->CheckCurrentEndDay(CurrentDay))
		{
			// only reaches this point of the current node is a end node
			if (Cast<UEndQuestInfo>(OnGoingChainQuest->CurrentNode->QuestInfo)->NextChainQuest != nullptr)
			{
				AddChainQuest(Cast<UEndQuestInfo>(OnGoingChainQuest->CurrentNode->QuestInfo)->NextChainQuest);
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

