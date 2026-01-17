// Shungen All Rights Reserved.


#include "Widgets/BluehorseTitleScreenWidget.h"

/**
 * タイトル画面用 UI Widget の入力処理。
 *
 * - NativeOnPreviewKeyDown を使用して UI 入力を最優先で処理する
 * - ゲームパッド操作によるメニュー選択を想定
 * - CurrentIndex を使って現在選択中の項目を管理する
 * - ログはデバッグ目的（入力・遷移確認用）
 *
 * 実際の見た目変更や画面遷移は Blueprint 側に委譲している
 */
FReply UBluehorseTitleScreenWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 入力されたキーをログ出力
	UE_LOG(LogTemp, Warning,
		TEXT("[TitleScreen] Key Pressed: %s"),
		*InKeyEvent.GetKey().GetDisplayName().ToString()
	);

	// --- 下入力：次のメニュー項目へ -------------------------------------
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

			// 現在のハイライト解除
			BP_OnHighlightChanged(CurrentIndex, false);

			// インデックス更新
			CurrentIndex += 1;

			// 新しい項目をハイライト
			BP_OnHighlightChanged(CurrentIndex, true);

			// 移動音を再生
			PlayUISound(MoveSound);
		}
		else
		{
			// 既に最下段の場合は無視
			UE_LOG(LogTemp, Verbose,
				TEXT("[TitleScreen] DPad Down ignored (Bottom reached)")
			);
		}
	}

	// --- 上入力：前のメニュー項目へ -------------------------------------
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

			// 現在のハイライト解除
			BP_OnHighlightChanged(CurrentIndex, false);

			// インデックス更新
			CurrentIndex -= 1;

			// 新しい項目をハイライト
			BP_OnHighlightChanged(CurrentIndex, true);

			// 移動音を再生
			PlayUISound(MoveSound);
		}
		else
		{
			// 既に最上段の場合は無視
			UE_LOG(LogTemp, Verbose,
				TEXT("[TitleScreen] DPad Up ignored (Top reached)")
			);
		}
	}

	// --- 決定入力：選択中の項目を確定 -----------------------------------
	else if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[TitleScreen] FaceButton Bottom (Decide) | Index: %d"),
			CurrentIndex
		);

		// Blueprint 側で決定処理を実行
		BP_OnFaceButtonBottom();

		// 決定音を再生
		PlayUISound(DecideSound);
	}

	// その他の入力は親クラスへ委譲
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}