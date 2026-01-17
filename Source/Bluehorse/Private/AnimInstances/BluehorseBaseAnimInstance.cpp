// Shungen All Rights Reserved.


#include "AnimInstances/BluehorseBaseAnimInstance.h"
#include "BluehorseFunctionLibrary.h"

// この AnimInstance の Owner（Pawn）が指定した GameplayTag を保持しているかを判定する
bool UBluehorseBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
    // AnimInstance に紐づく Pawn を取得
    if (APawn* OwningPwan = TryGetPawnOwner())
    {
        // Pawn が指定タグを保持しているかを判定
        return UBluehorseFunctionLibrary::NativeDoesActorHaveTag(OwningPwan, TagToCheck);
    }

    // Pawn が取得できない場合（初期化前など）は false とする
    return false;
}
