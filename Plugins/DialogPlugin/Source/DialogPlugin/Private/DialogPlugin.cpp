#include "DialogPlugin.h"
#include "DialogAssetAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphPin.h"
#include "KismetPins/SGraphPinColor.h"
#include "GraphEditorActions.h"

#define LOCTEXT_NAMESPACE "FDialogPluginModule"

class SDialogGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:
	virtual FSlateColor GetPinColor() const override { return FLinearColor(0.2f, 1.0f, 0.2f, 1.0f); }
};

class SDialogStartGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogStartGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:
	virtual FSlateColor GetPinColor() const override { return FLinearColor(1.f, 0.2f, 0.2f, 1.0f); }
};

class SDialogEndGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogEndGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InPin);
	}
protected:
	virtual FSlateColor GetPinColor() const override { return FLinearColor(0.2f, 0.2f, 1.0f, 1.0f); }
};

struct FDialogCustomPinFactory : public FGraphPanelPinFactory
{
	virtual ~FDialogCustomPinFactory() {}

	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override
	{
		if (FName(TEXT("DialogPin"))      == Pin->PinType.PinSubCategory) return SNew(SDialogGraphPin,      Pin);
		if (FName(TEXT("DialogStartPin")) == Pin->PinType.PinSubCategory) return SNew(SDialogStartGraphPin, Pin);
		if (FName(TEXT("DialogEndPin"))   == Pin->PinType.PinSubCategory) return SNew(SDialogEndGraphPin,   Pin);
		return nullptr;
	}
};

void FDialogPluginModule::StartupModule()
{
	FGraphEditorCommands::Register();

	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(FName("DialogAssets"), FText::FromString(TEXT("Dialog Assets")));
	TSharedPtr<FDialogAssetAction> DialogAsset = MakeShareable(new FDialogAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(DialogAsset.ToSharedRef());

	StyleSet = MakeShareable(new FSlateStyleSet(TEXT("DialogSystemEditorStyle")));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("DialogPlugin");
	if (Plugin.IsValid())
	{
		StyleSet->SetContentRoot(Plugin->GetContentDir());
	}

	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("DialogIcon"),          TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* IconBrush      = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("DialogIcon"),          TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* AddPinIcon     = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeAddPinIcon"),      TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* DeletePinIcon  = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeDeletePinIcon"),   TEXT(".png")), FVector2D(120.f, 120.f));
	FSlateImageBrush* DeleteNodeIcon = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeDeleteNodeIcon"),  TEXT(".png")), FVector2D(120.f, 120.f));

	StyleSet->Set(TEXT("ClassThumbnail.Dialog"),        ThumbnailBrush);
	StyleSet->Set(TEXT("ClassIcon.Dialog"),             IconBrush);
	StyleSet->Set(TEXT("DialogEditor.NodeAddPinIcon"),    AddPinIcon);
	StyleSet->Set(TEXT("DialogEditor.NodeDeletePinIcon"), DeletePinIcon);
	StyleSet->Set(TEXT("DialogEditor.NodeDeleteNodeIcon"),DeleteNodeIcon);
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	CustomPinFactory = MakeShareable(new FDialogCustomPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(CustomPinFactory);
}

void FDialogPluginModule::ShutdownModule()
{
	FGraphEditorCommands::Unregister();
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	FEdGraphUtilities::UnregisterVisualPinFactory(CustomPinFactory);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDialogPluginModule, DialogPlugin)
