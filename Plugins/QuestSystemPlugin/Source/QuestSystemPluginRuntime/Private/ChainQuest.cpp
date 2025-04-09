

#include "ChainQuest.h"

#include "ChainQuestHandler.h"
#include "QuestTagsManager.h"

FChainQuestHandler UChainQuest::GetHandler() const
{
	return FChainQuestHandler(this);
}

UChainQuest::UChainQuest()
{
	
}
