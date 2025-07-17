

#include "BasicActor.h"


ABasicActor::ABasicActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABasicActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABasicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

