
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BasicActor.generated.h"

UCLASS()
class NPCMANAGESYSTEM_API ABasicActor : public AActor
{
	GENERATED_BODY()

public:
	ABasicActor();


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess=true))
	FGameplayTag ItemTag;
	
};
