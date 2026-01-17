// Shungen All Rights Reserved.


#include "Widgets/BluehorseInteractScreenWidget.h"

/**
 * ウィジェット上でのキー入力をプレビュー段階で処理する。
 *
 * - NativeOnPreviewKeyDown は通常の入力処理より前に呼ばれる
 * - ここではゲームパッドの上ボタン入力でインタラクト処理を実行する
 */
FReply UBluehorseInteractScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// ゲームパッドの上ボタン
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Top)
	{
		// Blueprint 側で定義されたインタラクト処理を呼び出す
		BP_OnFaceButtonTop();

		// インタラクト時の UI 効果音を再生
		PlayUISound(InteractSound);
	}

	// その他の入力は親クラスに委譲
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
