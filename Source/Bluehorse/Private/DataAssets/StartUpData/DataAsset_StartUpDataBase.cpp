// Shungen All Rights Reserved.


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"

/**
 * StartUpData を AbilitySystemComponent（ASC）へ適用する。
 *
 * 初期アビリティ（能動/受動）と初期GameplayEffect（ステータス等）を ASC に付与する
 *
 * @param InASCToGive 付与先の ASC
 * @param ApplyLevel  付与する Ability / Effect のレベル（スケーリング用）
 */
void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	// ASC は必須（未初期化のまま呼ぶのは設計ミス）
	check(InASCToGive);

	// --- Abilities ----------------------------------------------------------
	// 付与時に即起動するタイプ
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);

	// 反応型アビリティを想定
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	// --- GameplayEffects ----------------------------------------------------
	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf < UGameplayEffect >& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			// CDO を Self に適用する（ローカル動作前提でも ASC 経由で統一）
			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()
			);
		}
	}

}

/**
 * Ability の配列を ASC に付与する共通処理。
 *
 * - AbilitySpec を生成し、SourceObject / Level 等を設定して GiveAbility する
 * - SourceObject に AvatarActor を入れておくことで、
 *   Ability 内で「誰がこの Ability を持っているか」を参照しやすくする
 *
 * @param InAbilitiesToGive 付与したい Ability クラス配列
 * @param InASCToGive       付与先の ASC
 * @param ApplyLevel        付与レベル
 */
void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UBluehorseGameplayAbility>>& InAbilitiesToGive, UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	// Ability を逐次処理して ASC に付与
	for (const TSubclassOf<UBluehorseGameplayAbility>& Ability : InAbilitiesToGive) // Abilityを逐次処理
	{
		if (!Ability) continue;

		// AbilitySpecを作成して必要な情報を登録
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		// ASC に登録
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
