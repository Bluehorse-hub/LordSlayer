// Shungen All Rights Reserved.


#include "Widgets/BluehorseMainMenuScreenWidget.h"

#include "BluehorseDebugHelper.h"

/**
 * メインメニュー画面での入力処理（ゲームパッド対応）。
 *
 * - 選択中のボタンはインデックス（CurrentIndex）で管理
 * - 見た目のハイライト変更や決定処理は Blueprint 側に委譲する
 */
FReply UBluehorseMainMenuScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// --- 下入力：次のメニュー項目へ -------------------------------------
	if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		// 配列範囲外に出ないようチェック
		if (CurrentIndex < NumButton - 1)
		{
			// 現在の項目のハイライトを解除
			BP_OnHighlightChanged(CurrentIndex, false);

			// インデックスを進める
			CurrentIndex += 1;

			// 新しい項目をハイライト
			BP_OnHighlightChanged(CurrentIndex, true);

			// 移動音を再生
			PlayUISound(MoveSound);
		}
	}

	// --- 上入力：前のメニュー項目へ -------------------------------------
	else if (InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		// 配列範囲外に出ないようチェック
		if (CurrentIndex > 0)
		{
			// 現在の項目のハイライトを解除
			BP_OnHighlightChanged(CurrentIndex, false);

			// インデックスを戻す
			CurrentIndex -= 1;

			// 新しい項目をハイライト
			BP_OnHighlightChanged(CurrentIndex, true);

			// 移動音を再生
			PlayUISound(MoveSound);
		}
	}

	// --- 決定入力：選択中の項目を確定 -----------------------------------
	else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		// Blueprint 側で決定処理を実行
		BP_OnFaceButtonBottom();

		// 決定音を再生
		PlayUISound(DecideSound);
	}

	// その他の入力は親クラスへ委譲
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
