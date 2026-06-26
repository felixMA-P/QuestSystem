#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "DialogHandler.h"
#include "DialogLineInfo.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "DialogWorldSubsystem.generated.h"

class UDialogDataAsset;
class UDialogInfoBase;

// Fires when the active line changes — either on dialog start or after a response is selected.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogLineChanged, const UDialog*, Dialog, UDialogLineInfo*, LineInfo);

// Fires when a dialog reaches an EndNode.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogEnded, const UDialog*, Dialog);

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

	friend class UDialogHandler;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

private:
	UPROPERTY()
	TArray<UDialogHandler*> Dialogs;

	UPROPERTY()
	TArray<UDialogHandler*> EndDialogs;

public:
	// Bind to this in your dialog Widget to react to each new line without polling.
	UPROPERTY(BlueprintAssignable, Category = "Dialog System")
	FOnDialogLineChanged OnDialogLineChanged;

	// Bind to this to close the dialog UI and trigger any post-dialog logic.
	UPROPERTY(BlueprintAssignable, Category = "Dialog System")
	FOnDialogEnded OnDialogEnded;

	// Tag container — used by StartCondition checks to query world/player state
	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	FGameplayTagContainer DialogGameplayTagsContainer;

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Add Gameplay Tags"))
	void AddGameplayTags(const TArray<FGameplayTag>& GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Initialize Dialogs"))
	void InitializeDialogs(const UDialogDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Add Dialog"))
	virtual void AddDialog(const UDialog* ToAddDialog);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Find Dialog"))
	bool FindDialog(const UDialog* InDialog);

	// Called by the UI when the player selects a response. Returns true if the dialog has ended.
	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Select Dialog Response"))
	bool SelectDialogResponse(const UDialog* InDialog, int32 ResponseIndex);

	// Returns the current line for a dialog so the UI can display speaker + text + response options.
	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Current Dialog Line"))
	UDialogLineInfo* GetCurrentDialogLine(const UDialog* InDialog);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get All Dialogs Info"))
	void GetAllDialogsInfo(FDialogsInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Dialog Lines Info"))
	void GetDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Ended Dialog Lines Info"))
	void GetEndedDialogLinesInfo(const UDialog* InDialog, FDialogLinesInfo& OutInfo);
};
