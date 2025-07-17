// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class NPCMANAGESYSTEM_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	ANPC();

	FGameplayTag NameTag;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
