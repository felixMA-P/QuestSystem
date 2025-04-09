// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestWorldSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "ChainQuest.h"
#include "ChainQuestGraph.h"
#include "ChainQuestHandler.h"
#include "DataAssetChainQuests.h"
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


void UQuestWorldSubsystem::InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer)
{

	for (const UChainQuest* ChainQuest : DataAssetInitializer->ChainQuests)
	{
		AddChainQuest(ChainQuest);
	}
	
}

void UQuestWorldSubsystem::CheckOnGoingQuestConditions()
{

	for (FChainQuestHandler & OnGoingChainQuest : ChainQuests)
	{
		OnGoingChainQuest.CheckCurrentNodeConditions(this);
	}
	
}

void UQuestWorldSubsystem::AddChainQuest(const UChainQuest* ToAddChainQuest)
{
	if (!ToAddChainQuest)
		return;
	
	if (ToAddChainQuest->StartCondition->GetDefaultObject<UCondition>()->CheckCondition(this))
	{
		ChainQuests.Add(ToAddChainQuest->GetHandler());
	}
	
}

bool UQuestWorldSubsystem::FindChainQuest(const UChainQuest* InChainQuest)
{
	return ChainQuests.FindByPredicate([InChainQuest](const FChainQuestHandler InHandler)
	{
		return InHandler.ChainQuest == InChainQuest;
	}) != nullptr;
}





