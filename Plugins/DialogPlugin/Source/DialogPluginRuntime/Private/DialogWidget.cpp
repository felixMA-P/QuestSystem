#include "DialogWidget.h"

#include "DialogLineInfo.h"
#include "DialogWorldSubsystem.h"

void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UDialogWorldSubsystem* Subsystem = GetDialogSubsystem();
	if (Subsystem)
	{
		Subsystem->OnDialogLineChanged.AddDynamic(this, &UDialogWidget::HandleLineChanged);
		Subsystem->OnDialogEnded.AddDynamic(this, &UDialogWidget::HandleDialogEnded);
	}
	
	SetVisibility(ESlateVisibility::Hidden);
	
}

void UDialogWidget::NativeDestruct()
{
	UDialogWorldSubsystem* Subsystem = GetDialogSubsystem();
	if (Subsystem)
	{
		Subsystem->OnDialogLineChanged.RemoveDynamic(this, &UDialogWidget::HandleLineChanged);
		Subsystem->OnDialogEnded.RemoveDynamic(this, &UDialogWidget::HandleDialogEnded);
	}

	Super::NativeDestruct();
}

void UDialogWidget::SelectResponse(int32 ResponseIndex)
{
	if (UDialogWorldSubsystem* Subsystem = GetDialogSubsystem())
	{
		Subsystem->SelectDialogResponse(ResponseIndex);
	}
}

UDialogWorldSubsystem* UDialogWidget::GetDialogSubsystem() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UDialogWorldSubsystem>() : nullptr;
}

void UDialogWidget::HandleLineChanged(UDialogLineInfo* LineInfo)
{
	if (Visibility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	
	OnLineChanged(LineInfo);
}

void UDialogWidget::HandleDialogEnded()
{
	SetVisibility(ESlateVisibility::Hidden);
	OnDialogEnded();
}
