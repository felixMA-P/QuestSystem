// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NPCInfoRow.h"
#include "NPCSystemComponent.h"
#include "Misc/LowLevelTestAdapter.h"
#include "UObject/ConstructorHelpers.h"


UNpcManagerSubsystem::UNpcManagerSubsystem()
{
	// Real, designer-populated DataTable asset; create it in the editor at this path
	// (Right-click > Miscellaneous > Data Table > Row Struct: NPCInfoRow) if it doesn't exist yet.
	/*static ConstructorHelpers::FObjectFinder<UDataTable> NPCDataTableFinder(TEXT("/NPCManageSystem/Data/DT_NPCInfo.DT_NPCInfo"));
	if (NPCDataTableFinder.Succeeded())
	{
		NPCDataTable = NPCDataTableFinder.Object;
	}*/
}

void UNpcManagerSubsystem::AddNPC(AActor* NPC)
{
	CHECK(NPC);

	UNPCSystemComponent* Comp = NPC->GetComponentByClass<UNPCSystemComponent>();
	
	if(Comp)
	{
		if (Comp->Tag.IsValid())
		{
			const FGameplayTag& Tag = Comp->Tag;
			const FNPCInfoRow* FoundRow = nullptr;
			NPCDataTable->ForeachRow<FNPCInfoRow>(TEXT("GetNPCInfo"), [&Tag, &FoundRow](const FName& RowName, const FNPCInfoRow& Row)
			{
				if (!FoundRow && Row.Tag == Tag)
				{
					FoundRow = &Row;
				}
			});
			
			if (FoundRow)
			{
				Comp->SetNPCData(*FoundRow);
			}
			
			TagNPCsMap.Add(Comp->Tag ,NPC);
		}
#if WITH_EDITOR
		else
		{
			UE_LOG(LogTemp, Error, TEXT("The NPC: %s Doesn't have a tag set up"), *NPC->GetActorLabel());
		}
#endif
	}
}

void UNpcManagerSubsystem::RemoveNPC(const FGameplayTag NameTag)
{
	TagNPCsMap.Remove(NameTag);
}

AActor* UNpcManagerSubsystem::GetNPC(const FGameplayTag NameTag)
{
	AActor** NpcPtr = TagNPCsMap.Find(NameTag);

	if (!NpcPtr || !*NpcPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No NPC found for tag: %s"), *NameTag.GetTagName().ToString());
		return nullptr;
	}
	
	return *NpcPtr;
}

bool UNpcManagerSubsystem::GetNPCInfo(const FGameplayTag& Tag, FNPCInfoRow& OutInfo)
{
	if (!NPCDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetNPCInfo: NPCDataTable is not set on the NPC Manager Subsystem"));
		return false;
	}

	const FNPCInfoRow* FoundRow = nullptr;
	NPCDataTable->ForeachRow<FNPCInfoRow>(TEXT("GetNPCInfo"), [&Tag, &FoundRow](const FName& RowName, const FNPCInfoRow& Row)
	{
		if (!FoundRow && Row.Tag == Tag)
		{
			FoundRow = &Row;
		}
	});

	if (!FoundRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetNPCInfo: No row found for tag '%s' in DataTable '%s'"),
			*Tag.GetTagName().ToString(), *NPCDataTable->GetName());
		return false;
	}

	OutInfo = *FoundRow;
	return true;
}

void UNpcManagerSubsystem::MoveNPC(const FGameplayTag NameTag, const FVector& Location)
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

void UNpcManagerSubsystem::InitNpcList(const FActorsInitializedParams& ActorsInitializedParams)
{
	/*TArray<AActor*> OutNpcs;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutNpcs);

	for (AActor* Npc : OutNpcs)
	{
		AddNPC(Npc);
	}*/
}

void UNpcManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	/*GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UNpcManagerSubsystem::RegisterNPC));
	OnActorsInitializedDelegate = GetWorld()->OnActorsInitialized.Add(UWorld::FOnWorldInitializedActors::FDelegate::CreateUObject(this, &UNpcManagerSubsystem::InitNpcList));*/
}

void UNpcManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UNpcManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}
