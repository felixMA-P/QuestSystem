

#include "EndQuestResult.h"


AEndQuestResult::AEndQuestResult()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEndQuestResult::ExecuteResult()
{
	ExecuteResultEvent();
}


