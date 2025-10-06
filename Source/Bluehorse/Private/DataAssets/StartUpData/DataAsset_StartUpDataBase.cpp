// Shungen All Rights Reserved.


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	check(InASCToGive);

	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	// GameplayEffect��o�^���鏈��
	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf < UGameplayEffect >& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()
			);
		}
	}

}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UBluehorseGameplayAbility>>& InAbilitiesToGive, UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UBluehorseGameplayAbility>& Ability : InAbilitiesToGive) // Ability�𒀎�����
	{
		if (!Ability) continue;

		// AbilitySpec���쐬���ĕK�v�ȏ���o�^
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASCToGive->GiveAbility(AbilitySpec);
	}
}
