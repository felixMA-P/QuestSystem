// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

#include "Schemas/QuestGraphNodeBase.h"

namespace
{
	const TArray<FLinearColor>& GetQuestBranchPalette()
	{
		static const TArray<FLinearColor> Palette = {
			FLinearColor(0.90f, 0.30f, 0.30f),
			FLinearColor(0.30f, 0.55f, 0.90f),
			FLinearColor(0.95f, 0.75f, 0.15f),
			FLinearColor(0.40f, 0.80f, 0.40f),
			FLinearColor(0.75f, 0.40f, 0.85f),
			FLinearColor(0.20f, 0.80f, 0.80f),
			FLinearColor(0.90f, 0.55f, 0.20f),
			FLinearColor(0.85f, 0.40f, 0.60f),
		};
		return Palette;
	}

	const FString BranchPinPrefix = TEXT("BranchColor");

	// Walks LinkedTo[0] through any chain of non-compiler-relevant (reroute/knot) nodes and returns the pin on the
	// first real node found, in either direction (relies on GetPassThroughPin flipping input<->output for knots).
	UEdGraphPin* ResolveThroughKnots(UEdGraphPin* Pin, TSet<UEdGraphNode*>& Visited)
	{
		if (!Pin || !Pin->HasAnyConnections()) return nullptr;

		UEdGraphPin* Next = Pin->LinkedTo[0];
		UEdGraphNode* Owner = Next->GetOwningNode();

		if (Owner && !Owner->IsCompilerRelevant())
		{
			if (Visited.Contains(Owner)) return nullptr;
			Visited.Add(Owner);

			UEdGraphPin* PassThrough = Owner->GetPassThroughPin(Next);
			return PassThrough ? ResolveThroughKnots(PassThrough, Visited) : nullptr;
		}

		return Next;
	}
}

FLinearColor UQuestGraphNodeBase::GetBranchColor(int32 Index)
{
	const TArray<FLinearColor>& Palette = GetQuestBranchPalette();
	return Palette[Index % Palette.Num()];
}

void UQuestGraphNodeBase::TagPinWithBranchColor(UEdGraphPin* Pin, const FLinearColor& Color)
{
	if (!Pin) return;
	Pin->PinType.PinSubCategory = FName(*(BranchPinPrefix + Color.ToFColor(true).ToHex()));
}

bool UQuestGraphNodeBase::TryGetBranchColor(const FEdGraphPinType& PinType, FLinearColor& OutColor)
{
	const FString SubCategory = PinType.PinSubCategory.ToString();
	if (!SubCategory.StartsWith(BranchPinPrefix)) return false;

	OutColor = FLinearColor(FColor::FromHex(SubCategory.RightChop(BranchPinPrefix.Len())));
	return true;
}

void UQuestGraphNodeBase::SyncInputPinColors()
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction != EEdGraphPinDirection::EGPD_Input) continue;

		TSet<UEdGraphNode*> Visited;
		UEdGraphPin* Source = ResolveThroughKnots(Pin, Visited);

		FLinearColor Color;
		if (Source && TryGetBranchColor(Source->PinType, Color))
		{
			TagPinWithBranchColor(Pin, Color);
		}
		else
		{
			Pin->PinType.PinSubCategory = NAME_None;
		}
	}
}
