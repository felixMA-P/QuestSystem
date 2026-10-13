// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "NPCManagerSettings.h"

UNPCManagerSettings::UNPCManagerSettings()
{
	// The example table shipped in the plugin's Content folder.
	NPCDataTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/NPCManagerSystem/NPCManagerSystem/Data/DT_NPCInfo.DT_NPCInfo")));
}
