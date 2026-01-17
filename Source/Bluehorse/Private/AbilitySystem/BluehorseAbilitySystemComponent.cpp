// Shungen All Rights Reserved.


#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

// 入力タグ（InputTag）に紐づく GameplayAbility を検索して発動する
void UBluehorseAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	// 入力タグが無効なら何もしない（安全側に倒す）
	if (!InInputTag.IsValid()) return;

	// ASC が保持する「発動可能な AbilitySpec」を走査して、入力タグが一致するものを探す
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// この Spec に紐づく入力タグと一致しないならスキップ
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		// Toggleable 系入力（例：ロックオン）なら
		// すでに発動中の場合は再入力でキャンセルにする
		if (InInputTag.MatchesTag(BluehorseGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			// 通常は入力押下で起動を試みる
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

// 入力タグ（InputTag）の「解放」イベント
void UBluehorseAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	// 無効タグは無視
	if (!InInputTag.IsValid())
	{
		return;
	}

	// 「押しっぱなしでのみ成立する能力」かどうかを判定
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		const bool bMustBeHeld = AbilitySpec.DynamicAbilityTags.HasTagExact(BluehorseGameplayTags::Player_Ability_MustBeHeld) ||
			(AbilitySpec.Ability && AbilitySpec.Ability->AbilityTags.HasTagExact(BluehorseGameplayTags::Player_Ability_MustBeHeld));

		// MustBeHeld かつ発動中なら、入力解放でキャンセルする
		if (bMustBeHeld && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

// 武器の AbilitySet を ASC に付与する。
void UBluehorseAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FBluehorseHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	// 付与対象が空なら何もしない
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	for (const FBluehorseHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		// AbilitySet の内容が不正ならスキップ（InputTag 未設定などを想定）
		if (!AbilitySet.IsValid()) continue;

		for (const TSubclassOf<UBluehorseGameplayAbility>& AbilityClass : AbilitySet.AbilitiesToGrant)
		{
			// クラスが無効ならスキップ
			if (!*AbilityClass) continue;

			// Spec を作成して付与
			FGameplayAbilitySpec AbilitySpec(AbilityClass);

			// 付与元（誰が・何が付与したか）を GA 側で参照できるようにする
			AbilitySpec.SourceObject = GetAvatarActor();

			// 付与レベル（スケーリングやコスト計算などで使用されうる）
			AbilitySpec.Level = ApplyLevel;

			// 入力タグを Spec の DynamicAbilityTags に積む（入力→Ability の解決に使用）
			AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

			// 付与したハンドルを保持して、後で「この武器分だけ」確実に剥がせるようにする
			OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
		}
	}
}

// GrantHeroWeaponAbilities で付与した武器 Ability をまとめて剥がす
void UBluehorseAbilitySystemComponent::RemoveGtantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			// 有効なハンドルのみ削除
			ClearAbility(SpecHandle);
		}
	}

	// 呼び出し側が同じ配列を使い回す前提のため、ここで確実に破棄しておく
	InSpecHandlesToRemove.Empty();
}

// AbilityTag を持つ GameplayAbility を 1つ選び、発動を試みる。
bool UBluehorseAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	// 呼び出し側の契約：有効タグのみ渡す（無効なら開発中に即落として原因を明確化）
	check(AbilityTagToActivate.IsValid());

	// 指定タグに一致する起動可能 AbilitySpec を収集
	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	// 複数ある場合はランダムで 1つ選ぶ
	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		// 発動中なら何もしない
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}
