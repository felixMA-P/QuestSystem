#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "DialogInfoBase.h"
#include "UObject/Object.h"
#include "DialogHandler.generated.h"

class UDialogRuntimeNode;
class UDialogLineInfo;
class UDialogWorldSubsystem;

UCLASS()
class DIALOGPLUGINRUNTIME_API UDialogHandler : public UObject
{
	GENERATED_BODY()

	friend class UDialogWorldSubsystem;

	UPROPERTY()
	UDialog* Dialog = nullptr;

	UPROPERTY()
	UDialogRuntimeNode* CurrentNode = nullptr;

	UPROPERTY()
	TArray<UDialogInfoBase*> DialogInfos;

	// Advances by the player-chosen response index.
	// Returns true if the dialog has reached an EndNode.
	bool SelectResponse(int32 ResponseIndex, UWorld* World);

public:
	// Called immediately after NewObject — replaces the constructor.
	void Initialize(const UDialog* InDialog);

	const UDialog* GetDialog() const { return Dialog; }

	// Returns the current line info for the UI to display (nullptr if on a non-dialog node)
	UDialogLineInfo* GetCurrentDialogLine() const;
};
