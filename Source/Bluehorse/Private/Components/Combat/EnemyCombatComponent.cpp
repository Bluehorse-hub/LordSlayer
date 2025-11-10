// Shungen All Rights Reserved.


#include "Components/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"
#include "BluehorseFunctionLibrary.h"
#include "Interfaces/PawnHitEffectInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BluehorseDebugHelper.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	//TODO::Implement block check
	bool bIsValidBlock = false;
	bool bIsValidParry = false;

	const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Blocking);
	const bool bIsPlayerParrying = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Parrying);
	const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Invincible);
	const bool bIsMyAttackUnblockable = false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	if (bIsPlayerParrying && !bIsMyAttackUnblockable)
	{
		bIsValidParry = UBluehorseFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn(); // Attacker
	EventData.Target = HitActor;

	if (bIsPlayerInvincible) return;

	if (bIsValidParry)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			BluehorseGameplayTags::Player_Event_SuccessfulParry,
			EventData
		);
	}
	else if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			BluehorseGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			BluehorseGameplayTags::Shared_Event_MeleeHit,
			EventData
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
}
