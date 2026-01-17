// Shungen All Rights Reserved.


#include "Widgets/BluehorsePortalWidget.h"
#include "Kismet/GameplayStatics.h"

#include "BluehorseDebugHelper.h"

/**
 * ポータル用 UI Widget の入力処理。
 *
 * - NativeOnPreviewKeyDown を使用して UI 入力を優先的に処理する
 * - UI は「未選択状態」と「選択中状態」の2段階で構成されている
 *
 * 未選択状態：
 *   - 決定操作（FaceButton_Top）で選択モードに入る
 *
 * 選択中状態：
 *   - 左右入力で選択項目を切り替え
 *   - 決定操作（FaceButton_Bottom）で現在の選択を確定
 */
FReply UBluehorsePortalWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{	
	// --- 選択モード中の入力処理 ------------------------------------------
	if (IsSelectWidget)
	{
		// 右入力：次の項目へ
		if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Right)
		{
			if (CurrentIndex < NumButton - 1)
			{
				// 現在のハイライト解除
				BP_OnHighlightChanged(CurrentIndex, false);

				// インデックス更新
				CurrentIndex += 1;

				// 新しい項目をハイライト
				BP_OnHighlightChanged(CurrentIndex, true);

				// 移動音再生
				PlayUISound(MoveSound);
			}
		}

		// 左入力：前の項目へ
		else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Left)
		{
			if (CurrentIndex > 0)
			{
				// 現在のハイライト解除
				BP_OnHighlightChanged(CurrentIndex, false);

				// インデックス更新
				CurrentIndex -= 1;

				// 新しい項目をハイライト
				BP_OnHighlightChanged(CurrentIndex, true);

				// 移動音再生
				PlayUISound(MoveSound);
			}
		}

		// 決定入力：現在の選択を確定
		else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
		{
			// Blueprint 側で選択確定処理を実行
			BP_OnFaceButtonBottom();

			// 決定音再生
			PlayUISound(DecideSound);
		}
	}

	// --- 未選択状態の入力処理 --------------------------------------------
	else
	{
		// インタラクト入力で選択モードへ移行
		if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Top)
		{
			// Blueprint 側でインタラクト処理を実行
			BP_OnFaceButtonTop();

			// 選択モードへ移行
			IsSelectWidget = true;

			// 初期選択項目をハイライト
			BP_OnHighlightChanged(CurrentIndex, true);

			// インタラクト音再生
			PlayUISound(InteractSound);
		}
	}

	// その他の入力は親クラスへ委譲
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
