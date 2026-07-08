#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NPCInfoRow.generated.h"

class USkeletalMesh;
class UTexture2D;

USTRUCT(BlueprintType)
struct NPCMANAGESYSTEM_API FNPCInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	// Identifying tag for this NPC; should match the row name used to look it up.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Info")
	FGameplayTag Tag;

	// Skeletal mesh used for 3D representation (e.g. in 3D widgets or previews)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Info")
	TSoftObjectPtr<USkeletalMesh> Mesh3D;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Info")
	TSoftObjectPtr<UTexture2D> Image2D;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Info")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Info")
	FText Description;
};
