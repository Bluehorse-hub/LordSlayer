// Shungen All Rights Reserved.


#include "Widgets/BluehorsePortalWidget.h"
#include "Kismet/GameplayStatics.h"

#include "BluehorseDebugHelper.h"

FReply UBluehorsePortalWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{	
	if (IsSelectWidget)
	{

		if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Right)
		{
			if (CurrentIndex < NumButton - 1)
			{
				BP_OnHighlightChanged(CurrentIndex, false);
				CurrentIndex += 1;
				BP_OnHighlightChanged(CurrentIndex, true);

				PlayUISound(MoveSound);
			}
		}

		else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Left)
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
	}

	else
	{
		if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Top)
		{
			BP_OnFaceButtonTop();
			IsSelectWidget = true;
			BP_OnHighlightChanged(CurrentIndex, true);

			PlayUISound(InteractSound);
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
