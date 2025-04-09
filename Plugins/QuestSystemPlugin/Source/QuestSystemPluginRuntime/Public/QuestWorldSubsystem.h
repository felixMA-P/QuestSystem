// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChainQuestHandler.h"
#include "QuestWorldSubsystem.generated.h"


class UDataAssetChainQuests;
class UChainQuest;
/**
 * 
 */
UCLASS()
class QUESTSYSTEMPLUGINRUNTIME_API UQuestWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class UQuestSystemComponent;
	friend FChainQuestHandler;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

private:
	
	TArray<FChainQuestHandler> ChainQuests;

public:

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	virtual void CheckOnGoingQuestConditions();

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	virtual void AddChainQuest(const UChainQuest* ToAddChainQuest);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	bool FindChainQuest(const UChainQuest* InChainQuest);
	
};
