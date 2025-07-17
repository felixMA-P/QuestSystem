// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NPCSystemComponent.h"
#include "Misc/LowLevelTestAdapter.h"


void UNpcManagerSubsystem::InitNPCS()
{
	TArray<AActor*> OutNpcs;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutNpcs);

	for (AActor* Npc : OutNpcs)
	{
		AddNPC(Npc);
	}
}

void UNpcManagerSubsystem::AddNPC(AActor* NPC)
{
	CHECK(NPC);

	UNPCSystemComponent* Comp = NPC->GetComponentByClass<UNPCSystemComponent>();
	
	if(Comp && Comp->Tag.IsValid())
	{
		TagNPCsMap.Add(Comp->Tag ,NPC);
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

AActor* UNpcManagerSubsystem::GetNPC(const FGameplayTag& NameTag)
{
	AActor** NpcPtr = TagNPCsMap.Find(NameTag);

	if (!NpcPtr || !*NpcPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No NPC found for tag: %s"), *NameTag.GetTagName().ToString());
		return nullptr;
	}
	
	return *NpcPtr;
}

void UNpcManagerSubsystem::MoveNPC(const FGameplayTag& NameTag, const FVector& Location)
{
	AActor* NpcPtr = GetNPC(NameTag);

	check(NpcPtr);
	
	NpcPtr->SetActorLocation(Location);
}

void UNpcManagerSubsystem::RegisterNPC(AActor* Actor)
{
	if (Actor)
	{
		AddNPC(Actor);
	}
}

void UNpcManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitNPCS();
	GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UNpcManagerSubsystem::RegisterNPC));
}

void UNpcManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UNpcManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
