// Shungen All Rights Reserved.


#include "Widgets/BluehorseDoorWidget.h"
#include "Interact/BluehorseDoorInteract.h"

void UBluehorseDoorWidget::InitDoor(ABluehorseDoorInteract* InDoor)
{
    // このWidgetを所有するドアを保持
    // （UI側で対象ドアを判断しなくて済むようにするため）
    OwnerDoor = InDoor;

    // ゲームパッド入力（Yボタン）を受け取るために
    // Widgetをフォーカス可能にしておく
    SetIsFocusable(true);
}

FReply UBluehorseDoorWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Xbox コントローラーの Y ボタン
    // （UE 内部キー名：Gamepad_FaceButton_Top）
    if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Top)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[DoorWidget] Gamepad Y pressed"));

        // 所有ドアが有効な場合のみ処理
        if (OwnerDoor.IsValid())
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[DoorWidget] OwnerDoor valid. Calling RequestToggle on %s"),
                *OwnerDoor->GetName());

            // UI → ドアへの通知
            // 実際の開閉ロジックはドア側に任せる
            OwnerDoor->RequestToggle();

            // インタラクト後は、このWidgetは不要になるため破棄
            RemoveFromParent();

            // 入力をここで消費し、他に伝播させない
            return FReply::Handled();
        }
        else
        {
            // デバッグ：ドア参照が切れている場合
            UE_LOG(LogTemp, Error,
                TEXT("[DoorWidget] OwnerDoor is INVALID"));
        }
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
