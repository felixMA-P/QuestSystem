#pragma once

#include "CoreMinimal.h"
#include "Dialog.h"
#include "DialogInfoBase.h"

class UDialogRuntimeNode;
class UDialogWorldSubsystem;

class FDialogHandler
{
	friend class UDialogWorldSubsystem;

	const UDialog* Dialog;
	UDialogRuntimeNode* CurrentNode = nullptr;
	TArray<const UDialogInfoBase*> DialogInfos;

	// Returns false if the dialog continues, true if the dialog ends
	bool CheckCurrentNodeConditions(UWorld* World);

public:
	const UDialog* GetDialog() const { return Dialog; }

	FDialogHandler(const UDialog* InDialog);
};
