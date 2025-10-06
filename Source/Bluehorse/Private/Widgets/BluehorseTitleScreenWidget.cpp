// Shungen All Rights Reserved.


#include "Widgets/BluehorseTitleScreenWidget.h"

FReply UBluehorseTitleScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		if (CurrentIndex < NumButton - 1)
		{
			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex += 1;
			BP_OnHighlightChanged(CurrentIndex, true);

		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		if (CurrentIndex > 0)
		{
			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex -= 1;
			BP_OnHighlightChanged(CurrentIndex, true);
		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		BP_OnFaceButtonBottom();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}