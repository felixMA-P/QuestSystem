// Fill out your copyright notice in the Description page of Project Settings.


#include "EntitiesSubsystem.h"

#include "BasicActor.h"
#include "Kismet/GameplayStatics.h"

void UEntitiesSubsystem::AddItem(ABasicActor* Actor)
{
	check(Actor);
	TagBasicActorMap.Add(Actor->ItemTag, Actor);
}

void UEntitiesSubsystem::RemoveItem(FGameplayTag EntityTag)
{
	TagBasicActorMap.Remove(EntityTag);
}

ABasicActor* UEntitiesSubsystem::GetItem(FGameplayTag EntityTag)
{

	ABasicActor** EntityPtr = TagBasicActorMap.Find(EntityTag);
	
	// checkf(EntityPtr, *FString::Printf(TEXT("There isn't a entity associated in the subsystem with this tag: %s"), *EntityTag.GetTagName().ToString()));
	// checkf(*EntityPtr, *FString::Printf(TEXT("There isn't a entity associated with this entity %s"), *EntityTag.GetTagName().ToString()));
	
	if (!EntityPtr || !*EntityPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Entity found for tag: %s"), *EntityTag.GetTagName().ToString());
		return nullptr;
	}
	
	return *EntityPtr;
	
}

void UEntitiesSubsystem::InitEntities()
{

	TArray<AActor*> OutEntities;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasicActor::StaticClass(), OutEntities);

	for (AActor* Entity : OutEntities)
	{
		ABasicActor* Ent = Cast<ABasicActor>(Entity);
		if (Ent && Ent->ItemTag.IsValid())
		{
			AddItem(Ent);
		}
	}
	
}

void UEntitiesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UEntitiesSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UEntitiesSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
