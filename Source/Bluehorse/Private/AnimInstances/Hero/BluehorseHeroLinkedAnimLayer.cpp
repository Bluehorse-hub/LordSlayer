// Shungen All Rights Reserved.


#include "AnimInstances/Hero/BluehorseHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/BluehorseHeroAnimInstance.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BluehorseDebugHelper.h"

void UBluehorseHeroLinkedAnimLayer::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningHeroCharacter = Cast<ABluehorseHeroCharacter>(TryGetPawnOwner());

    if (!OwningHeroCharacter) return;

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
    if (!Owner || !Owner->HasActorBegunPlay() || !MovementComp)
    {
        return; // ƒvƒŒƒrƒ…[’†‚â–¢‰Šú‰»‚Å‚Íˆ—‚µ‚È‚¢
    }
    if (!MovementComp)
    {
        Debug::Print(TEXT("ERROR"), FColor::Red, 1);
        return;
    }

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
    
    // FString DebugMsg = FString::Printf(TEXT("LocomotionType: %s"), *UEnum::GetValueAsString(LocomotionType));
    // Debug::Print(DebugMsg, FColor::Green, 2);

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

UBluehorseHeroAnimInstance* UBluehorseHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UBluehorseHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
