// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NPC.h"
#include "Misc/LowLevelTestAdapter.h"


void UNpcManagerSubsystem::InitNPCS()
{
	TArray<AActor*> OutNpcs;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), OutNpcs);

	for (AActor* Npc : OutNpcs)
	{
		ANPC* NPC = Cast<ANPC>(Npc);
		AddNPC(NPC);
	}
}

void UNpcManagerSubsystem::AddNPC(ANPC* NPC)
{
	CHECK(NPC);
	if(NPC->NameTag.IsValid())
	{
		TagNPCsMap.Add(NPC->NameTag ,NPC);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The NPC: %s Doesn't have a tag set up"), *NPC->GetActorLabel());
	}
}

void UNpcManagerSubsystem::RemoveNPC(const FGameplayTag& NameTag)
{
	TagNPCsMap.Remove(NameTag);
}

ANPC* UNpcManagerSubsystem::GetNPC(const FGameplayTag& NameTag)
{
	ANPC ** NpcPtr = TagNPCsMap.Find(NameTag);

	if (!NpcPtr || !*NpcPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No NPC found for tag: %s"), *NameTag.GetTagName().ToString());
		return nullptr;
	}
	
	return *NpcPtr;
}

void UNpcManagerSubsystem::MoveNPC(const FGameplayTag& NameTag, const FVector& Location)
{
	ANPC* NpcPtr = GetNPC(NameTag);

	check(NpcPtr);
	
	NpcPtr->SetActorLocation(Location);
}

void UNpcManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitNPCS();
}

void UNpcManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UNpcManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
