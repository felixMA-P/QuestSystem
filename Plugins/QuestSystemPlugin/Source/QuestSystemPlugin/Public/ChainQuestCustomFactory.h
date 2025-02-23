#pragma once

#include "CoreMinimal.h"
#include "ChainQuestCustomFactory.generated.h"

UCLASS()
class UChainQuestCustomFactory : public UFactory
{

	GENERATED_BODY()
	
public:
	UChainQuestCustomFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
	
};
