#include "Dialog.h"
#include "DialogHandler.h"

UDialog::UDialog()
{
}

FDialogHandler* UDialog::GetHandler() const
{
	return new FDialogHandler(this);
}
