

#include "ChainQuest.h"

#include "ChainQuestHandler.h"
#include "QuestTagsManager.h"

FChainQuestHandler* UChainQuest::GetHandler() const
{
	return new FChainQuestHandler(this);
}

UChainQuest::UChainQuest()
{
	
}
