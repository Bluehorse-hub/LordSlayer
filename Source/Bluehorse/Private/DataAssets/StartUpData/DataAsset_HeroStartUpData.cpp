// Shungen All Rights Reserved.


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"

/**
 * Hero 用 StartUpData を AbilitySystemComponent（ASC）へ適用する。
 *
 * - 親クラス（DataAsset_StartUpDataBase）で共通の初期 Ability / GameplayEffect を付与
 * - ここでは Hero 固有の「入力タグ付き Ability セット」を ASC に追加する
 *
 * @param InASCToGive 付与先の ASC
 * @param ApplyLevel  Ability / Effect に適用するレベル
 */
void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    // 共通初期化（Base Ability / Reactive Ability / GameplayEffect）
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    // --- Hero StartUp Ability Sets -----------------------------------------
    // 入力タグ（InputTag）と Ability をセットで管理し、
    // Hero の操作に直接紐づく Ability を初期状態で付与する
    for (const FBluehorseHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        // セット自体が無効な場合はスキップ
        if (!AbilitySet.IsValid()) continue;

        // セット内の Ability を逐次付与
        for (const TSubclassOf<UBluehorseGameplayAbility>& AbilityClass : AbilitySet.AbilitiesToGrant)
        {
            if (!*AbilityClass) continue;

            // AbilitySpec を生成
            FGameplayAbilitySpec AbilitySpec(AbilityClass);
            AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
            AbilitySpec.Level = ApplyLevel;

            // 入力タグを DynamicAbilityTags に付与
            // → InputComponent / AbilitySystem 側で Tag ベース入力処理が可能になる
            AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

            // ASC に登録
            InASCToGive->GiveAbility(AbilitySpec);
        }
    }
}