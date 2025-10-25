// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/Attacks/GA_HeroAttackMaster.h"
#include "BluehorseFunctionLibrary.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "BluehorseTypes/BluehorseStructTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"

void UGA_HeroAttackMaster::RemoveTagsOnEndAbility()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();

    if (Avatar)
    {
        for (const FGameplayTag& Tag : TagsToRemoveOnEnd)
        {
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Avatar, Tag))
            {
                UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, Tag);
            }
        }
    }
}

FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedRightHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedRightHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedLeftHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedLeftHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

void UGA_HeroAttackMaster::HandleApplyDamage(const FGameplayEventData& EventData)
{
    AActor* TargetActor = const_cast<AActor*>(EventData.Target.Get());

    if (!TargetActor)
    {
        return;
    }

    FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

    if (IsRightWeapon)
    {
        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedRightHandWeaponDamageSpecHandle());
    }
    else
    {
        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedLeftHandWeaponDamageSpecHandle());
    }

    if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
    {
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            TargetActor,
            BluehorseGameplayTags::Shared_Ability_HitReact,
            EventData
        );
    }
}

float UGA_HeroAttackMaster::GetMotionValueFromDataTable()
{
    const FHeroMotionDataRow* Row = MotionValueDataTable->FindRow<FHeroMotionDataRow>(
        AttackName,
        TEXT("ContextString")
    );

    if (!Row)
    {
        return 1.0f;
    }

    return Row->MotionValue;
}

void UGA_HeroAttackMaster::ComputeAttackDirection()
{
    ABluehorseHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero)
    {
        return;
    }

    FVector2D LastInput2D(0.f, 0.f);

    if (Hero->GetCharacterMovement())
    {
        FVector InputVec = Hero->GetLastMovementInputVector();
        LastInput2D = FVector2D(InputVec.X, InputVec.Y);
    }

    UKismetMathLibrary::Normalize2D(LastInput2D);
    FVector CachedAttackDirection = FVector(LastInput2D.X, LastInput2D.Y, 0.f);

    if (CachedAttackDirection.IsNearlyZero(0.0001f))
    {
        CachedAttackDirection = Hero->GetActorForwardVector();
    }

    UMotionWarpingComponent* MotionWarpingComp = Hero->GetMotionWarpingComponent();

    if (!MotionWarpingComp)
    {
        return;
    }

    const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CachedAttackDirection);

    MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(NormalAttackWarpTargetName, FVector::ZeroVector, TargetRotation);
}
