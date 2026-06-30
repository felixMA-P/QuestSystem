// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NPCSystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPCMANAGESYSTEM_API UNPCSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNPCSystemComponent();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "NPC System", meta = (AllowPrivateAccess=true))
	FGameplayTag Tag;
};
