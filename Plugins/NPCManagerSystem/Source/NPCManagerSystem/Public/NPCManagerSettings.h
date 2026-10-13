// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "NPCManagerSettings.generated.h"

/**
 *
 * Project-wide settings for the NPC Manager, under Project Settings > Plugins > NPC Manager System.
 * Saved to the project's DefaultGame.ini, so they survive plugin updates.
 *
 * Felix Martin Arroyo
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "NPC Manager System"))
class NPCMANAGERSYSTEM_API UNPCManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UNPCManagerSettings();

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

	// DataTable of FNPCInfoRow the subsystem reads NPC display data from. Rows are matched on
	// their Tag field. Leave empty if NPCs need no display data.
	// The table is added to the cook automatically, so nothing has to reference it.
	UPROPERTY(Config, EditAnywhere, Category = "NPC Info", meta = (RequiredAssetDataTags = "RowStructure=/Script/NPCManagerSystem.NPCInfoRow"))
	TSoftObjectPtr<UDataTable> NPCDataTable;
};
