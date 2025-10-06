// Shungen All Rights Reserved.


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/BluehorseHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/PawnHitEffectInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroEquippedRightHandWeapon() const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterEquippedRightHandWeapon());
}

ABluehorseHeroWeapon* UHeroCombatComponent::GetHeroEquippedLeftHandWeapon() const
{
	return Cast<ABluehorseHeroWeapon>(GetCharacterEquippedLeftHandWeapon());
}

float UHeroCombatComponent::GetHeroEquippedRightHandWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroEquippedRightHandWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

float UHeroCombatComponent::GetHeroEquippedLeftHandWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroEquippedLeftHandWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	// 既にヒットしている相手には重複して当たらないようにする
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	// ヒットした相手を記録する
	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Shared_Event_MeleeHit,
		Data
	);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);

	if (IPawnHitEffectInterface* HitEffectInterface = Cast<IPawnHitEffectInterface>(HitActor))
	{
		if (UNiagaraSystem* HitEffect = HitEffectInterface->GetHitEffect())
		{
			//UE_LOG(LogTemp, Log, TEXT("ImpactPoint: %s"), *HitResult.ImpactPoint.ToString());
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

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		BluehorseGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
}
