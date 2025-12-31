// Shungen All Rights Reserved.


#include "Widgets/BluehorseDoorWidget.h"
#include "Interact/BluehorseDoorInteract.h"

void UBluehorseDoorWidget::InitDoor(ABluehorseDoorInteract* InDoor)
{
    OwnerDoor = InDoor;

    // Widget がキー入力を受けるために必要
    SetIsFocusable(true);

    UE_LOG(LogTemp, Warning,
        TEXT("[DoorWidget] InitDoor called. OwnerDoor = %s"),
        OwnerDoor.IsValid() ? *OwnerDoor->GetName() : TEXT("NULL"));
}

FReply UBluehorseDoorWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey PressedKey = InKeyEvent.GetKey();

    UE_LOG(LogTemp, Warning,
        TEXT("[DoorWidget] KeyDown detected. Key = %s"),
        *PressedKey.GetDisplayName().ToString());

    if (PressedKey == EKeys::Gamepad_FaceButton_Top)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[DoorWidget] Gamepad Y pressed"));

        if (OwnerDoor.IsValid())
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[DoorWidget] OwnerDoor valid. Calling RequestToggle on %s"),
                *OwnerDoor->GetName());

            OwnerDoor->RequestToggle(); // ← “通知”の本体（ドア側で開閉）
            RemoveFromParent();
            return FReply::Handled();
        }
        else
        {
            UE_LOG(LogTemp, Error,
                TEXT("[DoorWidget] OwnerDoor is INVALID"));
        }
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
