// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NPCInfoRow.generated.h"

class USkeletalMesh;
class UTexture2D;

USTRUCT(BlueprintType)
struct NPCMANAGERSYSTEM_API FNPCInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	// Identifying tag for this NPC. Rows are matched on this field, not on the row name.
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
