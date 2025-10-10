// Shungen All Rights Reserved.


#include "Widgets/BluehorseMainMenuScreenWidget.h"

#include "BluehorseDebugHelper.h"

FReply UBluehorseMainMenuScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		if (CurrentIndex < NumButton - 1)
		{
			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex += 1;
			BP_OnHighlightChanged(CurrentIndex, true);

			PlayUISound(MoveSound);
		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		if (CurrentIndex > 0)
		{
			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex -= 1;
			BP_OnHighlightChanged(CurrentIndex, true);

			PlayUISound(MoveSound);
		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		BP_OnFaceButtonBottom();
		PlayUISound(DecideSound);
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
