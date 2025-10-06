// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Controllers/BluehorseHeroController.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGamePlayTags.h"

// GASでアビリティから対応するキャラクターを取得する関数
ABluehorseHeroCharacter* UBluehorseHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedBluehorseHeroCharacter.IsValid())
	{
		CachedBluehorseHeroCharacter = Cast<ABluehorseHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

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

FGameplayEffectSpecHandle UBluehorseHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag)
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
		InWeaponBaseDamage
	);
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, 1.f);
	}
	return EffectSpecHandle;
}
