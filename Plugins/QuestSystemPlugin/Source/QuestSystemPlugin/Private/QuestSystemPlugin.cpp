// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestSystemPlugin.h"
#include "ChainQuestAssetAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorActions.h"

#define LOCTEXT_NAMESPACE "FQuestSystemPluginModule"

/*
 * Custom graph pin class configures my custom pin
 * 
 */

class SQuestGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SQuestGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:

	virtual FSlateColor GetPinColor() const override { return FLinearColor(0.2f, 1.0f, 0.2f, 1.0f); }
	
};

class SQuestStartGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SQuestGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:

	virtual FSlateColor GetPinColor() const override { return FLinearColor(1.f, 0.2f, 0.2f, 1.0f); }
	
};

class SQuestEndGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SQuestGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:

	virtual FSlateColor GetPinColor() const override { return FLinearColor(0.2f, 0.2f, 1.0f, 1.0f); }
	
};


/*
 * Custom pin factory
 */
struct FCustomPinFactory : public FGraphPanelPinFactory
{
	virtual ~FCustomPinFactory() {}

	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override {
		if (FName(TEXT("CustomPin")) == Pin->PinType.PinSubCategory) {
			return SNew(SQuestGraphPin, Pin);
		}
		if (FName(TEXT("StartPin")) == Pin->PinType.PinSubCategory) {
			return SNew(SQuestStartGraphPin, Pin);
		}
		if (FName(TEXT("EndPin")) == Pin->PinType.PinSubCategory) {
			return SNew(SQuestEndGraphPin, Pin);
		}
		return nullptr;
	}
};

void FQuestSystemPluginModule::StartupModule()
{
	FGraphEditorCommands::Register();

	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(FName("QuestAssets"), FText::FromString(TEXT("Quest Assets")));
	TSharedPtr<FChainQuestAssetAction> ChainQuestAsset = MakeShareable( new FChainQuestAssetAction(AssetType) );
	AssetToolsModule.RegisterAssetTypeActions(ChainQuestAsset.ToSharedRef());

	// Set up the style set
	StyleSet = MakeShareable(new FSlateStyleSet(TEXT("QuestSystemEditorStyle")));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("QuestSystemPlugin");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	// Create the brushes for the different uses
	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("QuestIcon"), TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* IconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("QuestIcon"), TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* NodeAddPinIcon = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeAddPinIcon"), TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* NodeDeletePinIcon = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeDeleteNodeIcon"), TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* NodeDeleteNodeIcon = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeDeletePinIcon"), TEXT(".png")), FVector2D(120.f, 120.f));

	//Set up the brushes in the style set
	StyleSet->Set(TEXT("ClassThumbnail.ChainQuest"), ThumbnailBrush);
	StyleSet->Set(TEXT("ClassIcon.ChainQuest"), IconBrush);
	StyleSet->Set(TEXT("QuestEditor.NodeAddPinIcon"), NodeAddPinIcon);
	StyleSet->Set(TEXT("QuestEditor.NodeDeletePinIcon"), NodeDeletePinIcon);
	StyleSet->Set(TEXT("QuestEditor.NodeDeleteNodeIcon"), NodeDeleteNodeIcon);
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	//Create the pin factory and register it
	CustomPinFactory = MakeShareable(new FCustomPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(CustomPinFactory);
}

void FQuestSystemPluginModule::ShutdownModule()
{
	FGraphEditorCommands::Unregister();
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	FEdGraphUtilities::UnregisterVisualPinFactory(CustomPinFactory);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuestSystemPluginModule, QuestSystemPlugin)