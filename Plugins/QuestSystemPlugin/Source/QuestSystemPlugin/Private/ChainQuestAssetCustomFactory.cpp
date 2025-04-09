#include "ChainQuestAssetCustomFactory.h"
#include "ChainQuest.h"

UChainQuestAssetCustomFactory::UChainQuestAssetCustomFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UChainQuest::StaticClass();
}

UObject* UChainQuestAssetCustomFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UObject* Object = NewObject<UChainQuest>(InParent, InName, Flags);
	return Object;
}

bool UChainQuestAssetCustomFactory::CanCreateNew() const
{
	return true;
}
