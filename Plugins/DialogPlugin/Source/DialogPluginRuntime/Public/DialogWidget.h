// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogWidget.generated.h"

class UDialogLineInfo;
class UDialogWorldSubsystem;

/*
 * Base class for the dialog HUD. Inherit from this in a Widget Blueprint.
 *
 * On construct it binds automatically to UDialogWorldSubsystem.
 * Implement OnLineChanged to refresh your text/buttons, and OnEnded to hide the widget.
 * Call SelectResponse(index) from each response button's OnClicked event.
 */
UCLASS(Abstract, Blueprintable)
class DIALOGPLUGINRUNTIME_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Implement this to update the speaker text, dialog text, and response buttons.
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Widget")
	void OnLineChanged(UDialogLineInfo* LineInfo);

	// Implement this to hide or animate out the dialog widget.
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Widget")
	void OnDialogEnded();

	// Call this from each response button's OnClicked — pass the button's index in the list.
	UFUNCTION(BlueprintCallable, Category = "Dialog Widget")
	void SelectResponse(int32 ResponseIndex);

	UFUNCTION(BlueprintPure, Category = "Dialog Widget")
	UDialogWorldSubsystem* GetDialogSubsystem() const;

private:
	UFUNCTION()
	void HandleLineChanged(UDialogLineInfo* LineInfo);

	UFUNCTION()
	void HandleDialogEnded();
};
