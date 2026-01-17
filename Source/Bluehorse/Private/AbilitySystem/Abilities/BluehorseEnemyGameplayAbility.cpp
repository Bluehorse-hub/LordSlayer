// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseEnemyGameplayAbility.h"
#include "Characters/BluehorseEnemyCharacter.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGameplayTags.h"
#include "Controllers/BluehorseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// ActorInfo から EnemyCharacter を取得する
ABluehorseEnemyCharacter* UBluehorseEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    // まだキャッシュしていない場合のみ Cast を行う
    if (!CachedBluehorseEnemyCharacter.IsValid())
    {
        CachedBluehorseEnemyCharacter = Cast<ABluehorseEnemyCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedBluehorseEnemyCharacter.IsValid() ? CachedBluehorseEnemyCharacter.Get() : nullptr;
}

// ActorInfo から Enemy 専用 AIController を取得する
ABluehorseAIController* UBluehorseEnemyGameplayAbility::GetEnemyAIControllerFromActorInfo() const
{
    // すでにキャッシュがあればそれを返す
    if (!CachedBluehorseAIController.IsValid())
    {
        if (CurrentActorInfo)
        {
            AActor* Avator = CurrentActorInfo->AvatarActor.Get();
            if (Avator)
            {
                // Enemy は InstigatorController を AIController として運用する
                CachedBluehorseAIController = Cast<ABluehorseAIController>(Avator->GetInstigatorController());
            }
        }
    }

    return CachedBluehorseAIController.IsValid() ? CachedBluehorseAIController.Get() : nullptr;
}

//  EnemyCharacter が保持する CombatComponent を取得する。
UEnemyCombatComponent* UBluehorseEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
    return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

// Enemy 用のダメージ GameplayEffectSpec を生成する
FGameplayEffectSpecHandle UBluehorseEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
    // ダメージ用 GE クラスは必須
    check(EffectClass);

    // EffectContext を作成し、この Ability を起点としたダメージであることを明示
    FGameplayEffectContextHandle ContextHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());

    // Enemy の場合、Source / Instigator は同一 Actor として扱う
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    // GameplayEffectSpec を生成
    FGameplayEffectSpecHandle EffectSpecHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

    // SetByCaller で「基礎ダメージ」を注入
    // 実際の最終ダメージ計算は GE 側で行う
    EffectSpecHandle.Data->SetSetByCallerMagnitude(
        BluehorseGameplayTags::Shared_SetByCaller_BaseDamage,
        InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
    );

    return EffectSpecHandle;
}

// Enemy の Blackboard から Object 型の値を取得するユーティリティ
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
