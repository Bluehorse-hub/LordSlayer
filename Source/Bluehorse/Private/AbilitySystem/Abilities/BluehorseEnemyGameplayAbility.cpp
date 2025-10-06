// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseEnemyGameplayAbility.h"
#include "Characters/BluehorseEnemyCharacter.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGameplayTags.h"

ABluehorseEnemyCharacter* UBluehorseEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    if (!CachedBluehorseEnemyCharacter.IsValid())
    {
        CachedBluehorseEnemyCharacter = Cast<ABluehorseEnemyCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedBluehorseEnemyCharacter.IsValid() ? CachedBluehorseEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UBluehorseEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
    return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UBluehorseEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
    check(EffectClass);

    FGameplayEffectContextHandle ContextHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle EffectSpecHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

    EffectSpecHandle.Data->SetSetByCallerMagnitude(
        BluehorseGameplayTags::Shared_SetByCaller_BaseDamage,
        InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
    );

    return EffectSpecHandle;
}
