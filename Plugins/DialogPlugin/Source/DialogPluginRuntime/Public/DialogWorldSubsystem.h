#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "DialogHandler.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "DialogWorldSubsystem.generated.h"

class UDialogDataAsset;
class UDialogInfoBase;

USTRUCT(BlueprintType)
struct FDialogsInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	TArray<const UDialog*> Dialogs;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	TArray<const UDialog*> EndDialogs;
};

USTRUCT(BlueprintType)
struct FDialogLinesInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	TArray<const UDialogInfoBase*> DialogInfos;
};

UCLASS(BlueprintType, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class FDialogHandler;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

private:
	TArray<FDialogHandler*> Dialogs;
	TArray<FDialogHandler*> EndDialogs;

public:
	// Main tag container — game systems add tags here to represent world/player state used by conditions
	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	FGameplayTagContainer DialogGameplayTagsContainer;

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Add Gameplay Tags"))
	void AddGameplayTags(const TArray<FGameplayTag>& GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Initialize Dialogs"))
	void InitializeDialogs(const UDialogDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Check On Going Dialog Conditions"))
	virtual void CheckOnGoingDialogConditions();

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Add Dialog"))
	virtual void AddDialog(const UDialog* ToAddDialog);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Find Dialog"))
	bool FindDialog(const UDialog* InDialog);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get All Dialogs Info"))
	void GetAllDialogsInfo(FDialogsInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Dialog Lines Info"))
	void GetDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Ended Dialog Lines Info"))
	void GetEndedDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo);
};
