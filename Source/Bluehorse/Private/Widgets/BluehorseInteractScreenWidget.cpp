// Shungen All Rights Reserved.


#include "Widgets/BluehorseInteractScreenWidget.h"

FReply UBluehorseInteractScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Top)
	{
		BP_OnFaceButtonTop();
		PlayUISound(InteractSound);
	}
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
