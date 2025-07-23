// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityComponent.h"


// Sets default values for this component's properties
UEntityComponent::UEntityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	UE_LOG(LogTemp, Warning, TEXT("Entities System Component"));
	
}


// Called when the game starts
void UEntityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}


// Called every frame
void UEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

