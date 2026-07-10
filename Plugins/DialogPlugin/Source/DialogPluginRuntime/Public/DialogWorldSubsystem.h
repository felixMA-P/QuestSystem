#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "DialogHandler.h"
#include "DialogLineInfo.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "DialogWorldSubsystem.generated.h"

class UDialogDataAsset;

// Fires when the active line changes — either on dialog start or after a response is selected.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogLineChanged, UDialogLineInfo*, LineInfo);

// Fires when the dialog is loaded.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogStarted);

// Fires when the dialog reaches an EndNode.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogEnded);

UCLASS(BlueprintType)
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
	UDialogHandler* ActiveDialog = nullptr;
	
	UPROPERTY()
	const UDialogDataAsset* DialogsAsset;

public:
	// Bind to this in your dialog Widget to react to each new line without polling.
	UPROPERTY(BlueprintAssignable, Category = "Dialog System")
	FOnDialogLineChanged OnDialogLineChanged;

	// Bind to this to close the dialog UI and trigger any post-dialog logic.
	UPROPERTY(BlueprintAssignable, Category = "Dialog System")
	FOnDialogEnded OnDialogEnded;
	
	// Bind to this to close the dialog UI and trigger any post-dialog logic.
	UPROPERTY(BlueprintAssignable, Category = "Dialog System")
	FOnDialogStarted OnDialogStarted;

	// Tag container — used by StartCondition checks to query world/player state.
	UPROPERTY(BlueprintReadOnly, Category = "Dialog System")
	FGameplayTagContainer DialogGameplayTagsContainer;

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Add Gameplay Tags"))
	void AddGameplayTags(const TArray<FGameplayTag>& GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Initialize Dialogs"))
	void InitializeDialogs(const UDialogDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Start Dialog"))
	void StartDialog(const FGameplayTag & DialogGameplayTag);

	// Called by the UI when the player selects a response. Returns true if the dialog has ended.
	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Select Dialog Response"))
	bool SelectDialogResponse(int32 ResponseIndex);

	// Returns the current line so the UI can display speaker + text + response options.
	UFUNCTION(BlueprintCallable, Category = "Dialog System", meta = (DisplayName = "Get Current Dialog Line"))
	UDialogLineInfo* GetCurrentDialogLine();

private:
	// Shared initialization path: checks StartCondition, creates the handler, and fires delegates.
	void StartDialogInternal(UDialog* Dialog);
};
