// Shungen All Rights Reserved.


#include "AnimInstances/Hero/BluehorseHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/BluehorseHeroAnimInstance.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BluehorseDebugHelper.h"

// Linked Anim Layer の初期化
void UBluehorseHeroLinkedAnimLayer::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // このレイヤーが紐づく Pawn を HeroCharacter として取得
    OwningHeroCharacter = Cast<ABluehorseHeroCharacter>(TryGetPawnOwner());

    if (!OwningHeroCharacter) return;

    // 移動速度制御に必要な MovementComponent をキャッシュ
    if (OwningHeroCharacter)
    {
        MovementComp = OwningHeroCharacter->GetCharacterMovement();
    }

    if (!MovementComp)
    {
        return;
    }
}

void UBluehorseHeroLinkedAnimLayer::PreUpdateAnimation(float DeltaSeconds)
{
    Super::PreUpdateAnimation(DeltaSeconds);

    AActor* Owner = GetOwningActor();

    // プレビュー中・初期化前は安全に何もしない
    if (!Owner || !Owner->HasActorBegunPlay() || !MovementComp)
    {
        return;
    }
    if (!MovementComp)
    {
        Debug::Print(TEXT("ERROR"), FColor::Red, 1);
        return;
    }

    // --- GameplayTag から LocomotionType を決定 --------------------------------
    // Blocking と TargetLock は同時に成立し得るため、組み合わせ状態を最優先で評価する
    if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetOwningActor(), BluehorseGameplayTags::Player_Status_Blocking) &&
        UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetOwningActor(), BluehorseGameplayTags::Player_Status_TargetLock))
    {
        LocomotionType = ELocomotionType::BlockingAndTargetLocking;
    }
    else
    {
        if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetOwningActor(), BluehorseGameplayTags::Player_Status_Blocking))
        {
            LocomotionType = ELocomotionType::Blocking;
        }
        else
        {
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetOwningActor(), BluehorseGameplayTags::Player_Status_TargetLock))
            {
                LocomotionType = ELocomotionType::TargetLocking;
            }
            else
                {
                LocomotionType = ELocomotionType::Default;
            }
        }
    }

    // --- LocomotionType に応じて移動速度を同期 ----------------------------------
    // MaxWalkSpeed をここで更新することで、「ブロッキング中は遅い」などの挙動を即反映する
    switch (LocomotionType)
    {
    case ELocomotionType::Default:
        MovementComp->MaxWalkSpeed = DefaultMaxWalkSpeed;
        break;
    case ELocomotionType::Blocking:
        MovementComp->MaxWalkSpeed = BlockingMaxWalkSpeed;
        break;
    case ELocomotionType::TargetLocking:
        MovementComp->MaxWalkSpeed = TargetLockingMaxWalkSpeed;
        break;
    case ELocomotionType::BlockingAndTargetLocking:
        MovementComp->MaxWalkSpeed = BlockingAndTargetLockingMaxWalkSpeed;
        break;
    }
}

void UBluehorseHeroLinkedAnimLayer::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
}

// OwningComponent から HeroAnimInstance を取得するヘルパー
UBluehorseHeroAnimInstance* UBluehorseHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UBluehorseHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
