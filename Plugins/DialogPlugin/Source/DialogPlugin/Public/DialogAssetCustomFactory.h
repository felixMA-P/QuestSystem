// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogAssetCustomFactory.generated.h"

UCLASS()
class UDialogAssetCustomFactory : public UFactory
{
	GENERATED_BODY()

public:
	UDialogAssetCustomFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
};
