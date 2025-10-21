// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseEnemyGameplayAbility.h"
#include "Characters/BluehorseEnemyCharacter.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGameplayTags.h"
#include "Controllers/BluehorseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ABluehorseEnemyCharacter* UBluehorseEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    if (!CachedBluehorseEnemyCharacter.IsValid())
    {
        CachedBluehorseEnemyCharacter = Cast<ABluehorseEnemyCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedBluehorseEnemyCharacter.IsValid() ? CachedBluehorseEnemyCharacter.Get() : nullptr;
}

ABluehorseAIController* UBluehorseEnemyGameplayAbility::GetEnemyAIControllerFromActorInfo() const
{
    if (!CachedBluehorseAIController.IsValid())
    {
        if (CurrentActorInfo)
        {
            AActor* Avator = CurrentActorInfo->AvatarActor.Get();
            if (Avator)
            {
                CachedBluehorseAIController = Cast<ABluehorseAIController>(Avator->GetInstigatorController());
            }
        }
    }

    return CachedBluehorseAIController.IsValid() ? CachedBluehorseAIController.Get() : nullptr;
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

UObject* UBluehorseEnemyGameplayAbility::GetBlackboardValueAsObject(FName KeyName) const
{
    if (ABluehorseAIController* AIController = GetEnemyAIControllerFromActorInfo())
    {
        UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();

        if (BBComp)
        {
            return BBComp->GetValueAsObject(KeyName);
        }
    }
    return nullptr;
}
