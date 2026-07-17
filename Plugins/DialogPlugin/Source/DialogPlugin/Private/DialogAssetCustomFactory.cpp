// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "DialogAssetCustomFactory.h"
#include "Dialog.h"

UDialogAssetCustomFactory::UDialogAssetCustomFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UDialog::StaticClass();
}

UObject* UDialogAssetCustomFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UDialog>(InParent, InName, Flags);
}

bool UDialogAssetCustomFactory::CanCreateNew() const
{
	return true;
}
