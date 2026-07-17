// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.



#include "ChainQuest.h"

#include "ChainQuestHandler.h"

FChainQuestHandler* UChainQuest::GetHandler() const
{
	return new FChainQuestHandler(this);
}

UChainQuest::UChainQuest()
{
	
}
