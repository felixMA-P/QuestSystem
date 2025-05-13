// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChainQuestHandler.h"
#include "QuestWorldSubsystem.generated.h"

UENUM(BlueprintType)
enum ETypeOfCalendar
{
	ByRest,
	ByTime
};

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

	UPROPERTY()
	TEnumAsByte<ETypeOfCalendar> TypeOfCalendar;

	UPROPERTY()
	int CurrentDay = 0;

	UPROPERTY()
	TMap<const UChainQuest*, int> CalendarChainQuests;

	TArray<FChainQuestHandler*> ChainQuests;
	
	TArray<FChainQuestHandler*> EndChainQuests;

public:

	UPROPERTY(BlueprintReadOnly, Category = "Quest System")
	FGameplayTagContainer QuestGameplayTagsContainer;
	
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void AddGameplayTags(const TArray<FGameplayTag>& GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	virtual void CheckOnGoingQuestConditions();

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	virtual void AddChainQuest(const UChainQuest* ToAddChainQuest);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	bool FindChainQuest(const UChainQuest* InChainQuest);
	
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void AddChainQuestToCalendar(const UChainQuest* ChainQuest, const int Day);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void CheckCalendar();

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void CheckCalendarOnGoingQuests();

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void EndOfDay();
	
};
