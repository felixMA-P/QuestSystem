
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NPCInfoRow.h"
#include "Engine/DataTable.h"
#include "Subsystems/WorldSubsystem.h"
#include "NpcManagerSubsystem.generated.h"

class ANPC;

/**
 * 
 * This system have the count of the Main Characters of the game and the secondary Characters
 * 
 * Felix Martin Arroyo
 */
UCLASS(Blueprintable, BlueprintType)
class NPCMANAGESYSTEM_API UNpcManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UNpcManagerSubsystem();

	UFUNCTION(BlueprintCallable, Category = "NPCManageSystem")
	void AddNPC(AActor* NPC);

	UFUNCTION(BlueprintCallable, Category = "NPCManageSystem")
	void RemoveNPC(const FGameplayTag NameTag);

	UFUNCTION(BlueprintCallable, Category = "NPCManageSystem")
	AActor* GetNPC(const FGameplayTag NameTag);

	UFUNCTION(BlueprintCallable, Category = "NPCManageSystem")
	void MoveNPC(const FGameplayTag NameTag, const FVector& Location);

	// Looks up NPCDataTable using the tag as row name. Returns false if not found.
	UFUNCTION(BlueprintCallable, Category = "NPCManageSystem")
	bool GetNPCInfo(const FGameplayTag& Tag, FNPCInfoRow& OutInfo);

private:

	// DataTable with FNPCInfoRow rows; row name must match the NPC's GameplayTag (e.g. "NPC.Blacksmith")
	UPROPERTY()
	TObjectPtr<UDataTable> NPCDataTable;

	UPROPERTY()
	TMap<FGameplayTag, AActor*> TagNPCsMap;

	FDelegateHandle OnActorsInitializedDelegate;

protected:
	
	virtual void RegisterNPC(AActor* Actor);

	void InitNpcList(const FActorsInitializedParams& ActorsInitializedParams);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
};
