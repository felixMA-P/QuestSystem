// Fill out your copyright notice in the Description page of Project Settings.


#include "EntitiesSubsystem.h"
#include "EntityComponent.h"
#include "Kismet/GameplayStatics.h"

void UEntitiesSubsystem::AddItem(AActor* Actor)
{
	check(Actor);
	
	UEntityComponent* Comp = Actor->FindComponentByClass<UEntityComponent>();
	
	if (Comp)
	{
		if (Comp->Tag.IsValid())
		{
			TagBasicActorMap.Add(Comp->Tag, Actor);
		}
#if WITH_EDITOR
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tag is not valid check the value in the actor: %s"), *Actor->GetActorLabel());
		}
#endif
	}
}

void UEntitiesSubsystem::RemoveItem(const FGameplayTag EntityTag)
{
	TagBasicActorMap.Remove(EntityTag);
}

AActor* UEntitiesSubsystem::GetItem(const FGameplayTag EntityTag)
{

	AActor** EntityPtr = TagBasicActorMap.Find(EntityTag);
	
	// checkf(EntityPtr, *FString::Printf(TEXT("There isn't a entity associated in the subsystem with this tag: %s"), *EntityTag.GetTagName().ToString()));
	// checkf(*EntityPtr, *FString::Printf(TEXT("There isn't a entity associated with this entity %s"), *EntityTag.GetTagName().ToString()));
	
	if (!EntityPtr || !*EntityPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Entity found for tag: %s"), *EntityTag.GetTagName().ToString());
		return nullptr;
	}
	
	return *EntityPtr;
	
}

void UEntitiesSubsystem::RegisterNewEntity(AActor* Actor)
{
	if (Actor)
	{
		AddItem(Actor);
	}
}

void UEntitiesSubsystem::InitEntities(const FActorsInitializedParams& ActorsInitializedParams)
{
	TArray<AActor*> OutEntities;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutEntities);

	for (AActor* Entity : OutEntities)
	{
		AddItem(Entity);
	}
}

void UEntitiesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	OnActorsInitializedDelegate = GetWorld()->OnActorsInitialized.Add(UWorld::FOnWorldInitializedActors::FDelegate::CreateUObject(this, &UEntitiesSubsystem::InitEntities));
	OnRegisterNewEntityDelegate = GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UEntitiesSubsystem::RegisterNewEntity));
}

void UEntitiesSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UEntitiesSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
