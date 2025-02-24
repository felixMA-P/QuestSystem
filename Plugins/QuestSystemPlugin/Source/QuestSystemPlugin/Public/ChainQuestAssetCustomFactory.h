#pragma once

#include "CoreMinimal.h"
#include "ChainQuestAssetCustomFactory.generated.h"

UCLASS()
class UChainQuestAssetCustomFactory : public UFactory
{

	GENERATED_BODY()
	
public:
	UChainQuestAssetCustomFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
	
};
