// Shungen All Rights Reserved.


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/BluehorseHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/PawnHitEffectInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

// 指定した GameplayTag に対応する HeroWeapon を取得する
ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}
// 右手に装備中の HeroWeapon を取得する
ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroEquippedRightHandWeapon() const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterEquippedRightHandWeapon());
}
// 左手に装備中の HeroWeapon を取得する
ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroEquippedLeftHandWeapon() const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterEquippedLeftHandWeapon());
}

// 右手武器のダメージ量を指定レベルで取得する
float UHeroCombatComponent::GetHeroEquippedRightHandWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroEquippedRightHandWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

// 左手武器のダメージ量を指定レベルで取得する
float UHeroCombatComponent::GetHeroEquippedLeftHandWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroEquippedLeftHandWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

// 武器の当たり判定によってターゲットにヒットした際に呼ばれる
void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	// 既にヒットしている相手には重複して当たらないようにする
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	// 今回の攻撃でヒットした Actor を記録
	OverlappedActors.AddUnique(HitActor);

	// --- GameplayEvent: MeleeHit ------------------------------------------
	// ダメージ処理や派生挙動は Ability 側で受け取って処理する設計
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Shared_Event_MeleeHit,
		Data
	);

	// --- GameplayEvent: HitPause ------------------------------------------
	// ヒットストップ（演出）用のイベント
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);

	// --- Hit Effect / Sound ------------------------------------------------
	// ヒット演出は Interface 経由で取得（敵/オブジェクト差し替え対応）
	if (IPawnHitEffectInterface* HitEffectInterface = Cast<IPawnHitEffectInterface>(HitActor))
	{
		if (UNiagaraSystem* HitEffect = HitEffectInterface->GetHitEffect())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitEffect,
				HitResult.ImpactPoint,
				HitResult.ImpactNormal.Rotation(),
				FVector(1.f),
				true,
				true
			);
		}

		if (USoundBase* HitSound = HitEffectInterface->GetHitSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				HitSound,
				HitResult.ImpactPoint
			);
		}
	}
}

// 武器がターゲットから離れたタイミングで呼ばれる
void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
}
