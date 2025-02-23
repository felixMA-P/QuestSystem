// Plugin module base class

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"

class FQuestSystemPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FSlateStyleSet> StyleSet = nullptr;
	TSharedPtr<struct FCustomPinFactory> CustomPinFactory = nullptr;
	
};
