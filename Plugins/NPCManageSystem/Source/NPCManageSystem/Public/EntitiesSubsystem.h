// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EntitiesSubsystem.generated.h"

class ABasicActor;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NPCMANAGESYSTEM_API UEntitiesSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	void AddItem(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	void RemoveItem(const FGameplayTag EntityTag);

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	AActor* GetItem(const FGameplayTag EntityTag);
	
private:
	
	UPROPERTY()
	TMap<FGameplayTag, AActor*> TagBasicActorMap;
	
	FDelegateHandle OnActorsInitializedDelegate;
	FDelegateHandle OnRegisterNewEntityDelegate;

protected:

	
	virtual void RegisterNewEntity(AActor* Actor);

	void InitEntities(const FActorsInitializedParams& ActorsInitializedParams);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
