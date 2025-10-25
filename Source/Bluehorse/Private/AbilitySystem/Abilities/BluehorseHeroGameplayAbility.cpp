// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Controllers/BluehorseHeroController.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGamePlayTags.h"

// GASでアビリティから対応するキャラクターを取得する関数
ABluehorseHeroCharacter* UBluehorseHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// キャッシュが無効なら再取得（パフォーマンス向上のためWeakPtr使用）
	if (!CachedBluehorseHeroCharacter.IsValid())
	{
		CachedBluehorseHeroCharacter = Cast<ABluehorseHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	// キャッシュが有効なら返す、無効ならnullptr
	return CachedBluehorseHeroCharacter.IsValid() ? CachedBluehorseHeroCharacter.Get() : nullptr;
}

ABluehorseHeroController* UBluehorseHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedBluehorseHeroController.IsValid())
	{
		CachedBluehorseHeroController = Cast<ABluehorseHeroController>(CurrentActorInfo->PlayerController);
	}

	return CachedBluehorseHeroController.IsValid() ? CachedBluehorseHeroController.Get() : nullptr;
}

UHeroCombatComponent* UBluehorseHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

// ダメージ用のGameplayEffectSpecHandleを生成
FGameplayEffectSpecHandle UBluehorseHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, float MotionValue)
{
	
	check(EffectClass);

	// EffectContextを作成
	FGameplayEffectContextHandle ContextHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeEffectContext();

	// このアビリティ自体をEffectContextに登録（発動元情報として）
	ContextHandle.SetAbility(this);

	// 攻撃者（SourceObject, Instigator）を登録
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// GameplayEffectSpecを生成
	// 指定したEffectClassを元に、最終的なダメージ計算に使用されるSpecを作る
	FGameplayEffectSpecHandle EffectSpecHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// SetByCallerを使用して外部パラメータを設定
	// 攻撃時の基本ダメージを "Shared.SetByCaller.BaseDamage" にセット
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		BluehorseGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	// 攻撃種別（Light/Heavyなど）が有効なら、それもSetByCallerで渡す
	// GEExecCalc_DamageTakenの中でTagに応じた倍率を与える
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, MotionValue);
	}

	// 作成済みのEffectSpecを返す
	return EffectSpecHandle;
}
