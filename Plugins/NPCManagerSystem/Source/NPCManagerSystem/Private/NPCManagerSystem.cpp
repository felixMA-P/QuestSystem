// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "NPCManagerSystem.h"
#include "NPCManagerLog.h"

#if WITH_EDITOR
#include "GameDelegates.h"
#include "NPCManagerSettings.h"
#endif

DEFINE_LOG_CATEGORY(LogNPCManager);

#define LOCTEXT_NAMESPACE "FNPCManagerSystemModule"

void FNPCManagerSystemModule::StartupModule()
{
#if WITH_EDITOR
	ModifyCookHandle = FGameDelegates::Get().GetModifyCookDelegate().AddRaw(this, &FNPCManagerSystemModule::ModifyCook);
#endif
}

void FNPCManagerSystemModule::ShutdownModule()
{
#if WITH_EDITOR
	FGameDelegates::Get().GetModifyCookDelegate().Remove(ModifyCookHandle);
#endif
}

#if WITH_EDITOR
void FNPCManagerSystemModule::ModifyCook(TConstArrayView<const ITargetPlatform*> TargetPlatforms, TArray<FName>& PackagesToCook, TArray<FName>& PackagesToNeverCook)
{
	const TSoftObjectPtr<UDataTable>& Table = GetDefault<UNPCManagerSettings>()->NPCDataTable;

	if (!Table.IsNull())
	{
		PackagesToCook.AddUnique(Table.ToSoftObjectPath().GetLongPackageFName());
	}
}
#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNPCManagerSystemModule, NPCManagerSystem)
