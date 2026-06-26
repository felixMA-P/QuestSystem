#include "Dialog.h"
#include "DialogHandler.h"

UDialog::UDialog()
{
}

UDialogHandler* UDialog::GetHandler(UObject* Outer) const
{
	UDialogHandler* Handler = NewObject<UDialogHandler>(Outer);
	Handler->Initialize(this);
	return Handler;
}
