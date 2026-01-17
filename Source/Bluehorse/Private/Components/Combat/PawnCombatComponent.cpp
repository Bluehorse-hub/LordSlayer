// Shungen All Rights Reserved.


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/BluehorseWeaponBase.h"
#include "Components/BoxComponent.h"

#include "BluehorseDebugHelper.h"

// 武器をTagとペアにして登録する関数
void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister, ABluehorseWeaponBase* InWeaponToRegister, EWeaponSlot Slot)
{
	// Tagの重複登録を防ぐ
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	// 登録処理
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// Spawn時に当たり判定系の関数をバインドしておく
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);

	// Spawn時に左右で登録する処理(BP側で登録することが多いのでサブ的な機能)
	switch (Slot)
	{
	case EWeaponSlot::PreEquip:
		break;
	case EWeaponSlot::LeftHand:
		EquippedLeftHandWeaponTag = InWeaponTagToRegister;
		break;
	case EWeaponSlot::RightHand:
		EquippedRightHandWeaponTag = InWeaponTagToRegister;
		break;
	default:
		break;
	}
}

// Tagから武器を取得する関数
ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (ABluehorseWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

// 現在装備中（CurrentEquippedWeaponTag）の武器を取得する
ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

// 右手装備武器を取得する
ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterEquippedRightHandWeapon() const
{
	if (!EquippedRightHandWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(EquippedRightHandWeaponTag);
}

// 左手装備武器を取得する
ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterEquippedLeftHandWeapon() const
{
	if (!EquippedLeftHandWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(EquippedLeftHandWeaponTag);
}

// 武器の当たり判定（Collision）を ON/OFF する
void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	// 現状は左右手武器のコリジョン切り替えのみ対応
	if (ToggleDamageType == EToggleDamageType::EquippedRightHandWeapon || ToggleDamageType == EToggleDamageType::EquippedLeftHandWeapon)
	{

		ABluehorseWeaponBase* WeaponToToggle;

		// 対象武器を決定
		if (ToggleDamageType == EToggleDamageType::EquippedRightHandWeapon)
		{
			WeaponToToggle = GetCharacterEquippedRightHandWeapon();
		}
		else
		{
			WeaponToToggle = GetCharacterEquippedLeftHandWeapon();
		}

		// 設計上、攻撃時に対象武器が無いのは不正なのでチェック
		check(WeaponToToggle);

		if (bShouldEnable)
		{
			// 攻撃開始：当たり判定を有効化（Overlap などの Query のみ）
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			// 攻撃終了：当たり判定を無効化
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// 攻撃終了時にはそれまでのヒットした相手の記録を破棄する
			OverlappedActors.Empty();
			WeaponToToggle->IgnoreActors.Empty();
		}
	}

	// TODO: Handle body collision box
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	// 派生（HeroCombat / EnemyCombat）で具体実装する
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	// 派生（HeroCombat / EnemyCombat）で具体実装する
}


