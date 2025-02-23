#include "ChainQuestCustomFactory.h"
#include "ChainQuest.h"

UChainQuestCustomFactory::UChainQuestCustomFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UChainQuest::StaticClass();
}

UObject* UChainQuestCustomFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UChainQuest>(InParent, InName, Flags);;
}

bool UChainQuestCustomFactory::CanCreateNew() const
{
	return true;
}
