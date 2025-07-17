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
UCLASS()
class NPCMANAGESYSTEM_API UEntitiesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	void AddItem(ABasicActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	void RemoveItem(FGameplayTag EntityTag);

	UFUNCTION(BlueprintCallable, Category = "EntitiesSystem")
	ABasicActor* GetItem(FGameplayTag EntityTag);
	
private:
	
	UPROPERTY()
	TMap<FGameplayTag, ABasicActor*> TagBasicActorMap;

	
protected:

	virtual void InitEntities();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
