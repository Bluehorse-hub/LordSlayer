// Shungen All Rights Reserved.


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BluehorseEnemyGameplayAbility.h"

/**
 * Enemy 用 StartUpData を AbilitySystemComponent（ASC）へ適用する。
 *
 * - 親クラス（DataAsset_StartUpDataBase）で共通の初期 Ability / GameplayEffect を付与
 * - ここでは Enemy 固有の戦闘 Ability と EnemyTypeTag を追加で付与する
 *
 * @param InASCToGive 付与先の ASC
 * @param ApplyLevel  Ability / Effect に適用するレベル
 */
void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	// 共通初期化（Base Ability / Reactive Ability / GameplayEffect）
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	// --- Enemy Combat Abilities --------------------------------------------
	// Enemy 専用の戦闘アビリティを付与する
	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf<UBluehorseEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			// AbilitySpec を生成
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			// ASC に登録
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}

	// --- Enemy Type Tag -----------------------------------------------------\
	// 敵の種類を示す GameplayTag を付与
	// LooseGameplayTag として追加することで、常時参照可能な属性として扱う
	if (EnemyTypeTag.IsValid())
	{
		InASCToGive->AddLooseGameplayTag(EnemyTypeTag);
	}
}
