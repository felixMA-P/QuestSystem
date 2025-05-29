// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChainQuestHandler.h"
#include "ChainQuest.h"
#include "QuestWorldSubsystem.generated.h"

class UQuestInfo;

UENUM(BlueprintType)
enum ETypeOfCalendar
{
	ByRest,
	ByTime
};

USTRUCT(BlueprintType)
struct FChainsQuestsInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TArray <const UChainQuest*> ChainQuests;

	UPROPERTY(BlueprintReadOnly)
	TArray <const UChainQuest*> EndChainQuests;
	
};

USTRUCT(BlueprintType)
struct FQuestsInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TArray <const UQuestInfoBase*> QuestsInfo;
	
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

	// This property has all the Tags added to the system it is the main way to create conditions
	UPROPERTY(BlueprintReadOnly, Category = "Quest System")
	FGameplayTagContainer QuestGameplayTagsContainer;

	// Adds the tags to the GamplayTagContainer QuestGameplayTagsContainer
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Add Gameplay Tags", ToolTip = "Adds the tags to the GamplayTagContainer QuestGameplayTagsContainer"))
	void AddGameplayTags(const TArray<FGameplayTag>& GameplayTags);

	// Adds and initialize the chainquests in the system
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Initialize Chain Quests", ToolTip = "Adds and initialize the chain quests in the system"))
	void InitializeChainQuests(const UDataAssetChainQuests* DataAssetInitializer);

	// Check the conditions on the not finished quests
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Check On Going Quests Conditions", ToolTip = "Check the conditions of the on going quests"))
	virtual void CheckOnGoingQuestConditions();

	// Adds a chain quest to the system and initializes the chain quest
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Add Chain Quest", ToolTip = "Adds a chain quest in the system and initializes the chain quest"))
	virtual void AddChainQuest(const UChainQuest* ToAddChainQuest);

	// Checks if the chain quest exists in the on going quests
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Find Chain Quest", ToolTip = "Retruns if a Chain Quest allready exists in the system"))
	bool FindChainQuest(const UChainQuest* InChainQuest);

	// Adds a chain quest to the Map CalendarChainQuests, 
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Add Chain Quest To Calendar",
		ToolTip = "Adds a chain quest to the calendar, when the system reaches the input day, the system will initilize the chain quest"))
	void AddChainQuestToCalendar(const UChainQuest* ChainQuest, const int Day);

	// Check the calendar map and adds the quest if the conditions are meet
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Find Chain Quest", ToolTip = "Retruns if a Chain Quest allready exists in the system"))
	void CheckCalendar();

	// Check the on going quests end day if the day has been reached the system advance the quest by the first condition
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "Check on going quests dates", ToolTip = "Check the end date of the on going chain quests"))
	void CheckCalendarOnGoingQuests();

	// Function to be called when the external time system requires this advance 1 day in the system
	UFUNCTION(BlueprintCallable, Category = "Quest System", meta = (DisplayName = "End Of Day", ToolTip = "Advance 1 day in the system"))
	void EndOfDay();

	// Gets the info of the chain quests
	UFUNCTION(BlueprintCallable ,Category = "Quest System", meta = (DisplayName = "Get All Chain Quests Info", ToolTip = "Returns two array with the chain quests"))
	void GetAllChainQuestsInfo(FChainsQuestsInfo & OutInfo);

	// Gets all the Quests form a chain quest
	UFUNCTION(BlueprintCallable ,Category = "Quest System", meta = (DisplayName = "Get Quests From Chain Quest", ToolTip = "Returns an array of quest info"))
	void GetChainQuestQuestsInfo(const UChainQuest* ChainQuest, FQuestsInfo & OutInfo);

	// Gets all the Quests form an ended chain quest
	UFUNCTION(BlueprintCallable ,Category = "Quest System", meta = (DisplayName = "Get Quests From Ended Chain Quest", ToolTip = "Returns an array of quest info"))
	void GetEndedChainQuestQuestsInfo(const UChainQuest* ChainQuest, FQuestsInfo & OutInfo);
	
	
};
