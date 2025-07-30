

#include "ChainQuest.h"

#include "ChainQuestHandler.h"

FChainQuestHandler* UChainQuest::GetHandler() const
{
	return new FChainQuestHandler(this);
}

UChainQuest::UChainQuest()
{
	
}
