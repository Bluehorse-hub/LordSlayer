// Shungen All Rights Reserved.


#include "Widgets/BluehorseTitleScreenWidget.h"

FReply UBluehorseTitleScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 入力されたキーをログ出力
	UE_LOG(LogTemp, Warning,
		TEXT("[TitleScreen] Key Pressed: %s"),
		*InKeyEvent.GetKey().GetDisplayName().ToString()
	);

	if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[TitleScreen] DPad Down | CurrentIndex: %d"),
			CurrentIndex
		);

		if (CurrentIndex < NumButton - 1)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[TitleScreen] Move Down -> %d -> %d"),
				CurrentIndex,
				CurrentIndex + 1
			);

			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex += 1;
			BP_OnHighlightChanged(CurrentIndex, true);

			PlayUISound(MoveSound);
		}
		else
		{
			UE_LOG(LogTemp, Verbose,
				TEXT("[TitleScreen] DPad Down ignored (Bottom reached)")
			);
		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[TitleScreen] DPad Up | CurrentIndex: %d"),
			CurrentIndex
		);

		if (CurrentIndex > 0)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[TitleScreen] Move Up -> %d -> %d"),
				CurrentIndex,
				CurrentIndex - 1
			);

			BP_OnHighlightChanged(CurrentIndex, false);
			CurrentIndex -= 1;
			BP_OnHighlightChanged(CurrentIndex, true);

			PlayUISound(MoveSound);
		}
		else
		{
			UE_LOG(LogTemp, Verbose,
				TEXT("[TitleScreen] DPad Up ignored (Top reached)")
			);
		}
	}

	else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[TitleScreen] FaceButton Bottom (Decide) | Index: %d"),
			CurrentIndex
		);

		BP_OnFaceButtonBottom();
		PlayUISound(DecideSound);
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}