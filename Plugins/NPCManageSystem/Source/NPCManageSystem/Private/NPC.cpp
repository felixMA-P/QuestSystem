// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"


ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

