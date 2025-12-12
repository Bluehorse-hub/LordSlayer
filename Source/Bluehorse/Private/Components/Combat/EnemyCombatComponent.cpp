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
	// すでにヒット処理を行った Actor に対しては再処理しない
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	// 今回ヒットした Actor を記録
	OverlappedActors.AddUnique(HitActor);

	// ブロック・パリィ判定結果
	bool bIsValidBlock = false;
	bool bIsValidParry = false;

	// プレイヤーの現在状態を GameplayTag で取得
	const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Blocking);
	const bool bIsPlayerParrying = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Parrying);
	const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Invincible);
	const bool bIsPlayerHajiki = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Player_Status_Hajiki);
	const bool bIsMyAttackUnblockable = false;

	// ブロック中かつガード可能な攻撃であれば、向きなどを考慮して有効判定を行う
	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	// パリィ中かつガード可能な攻撃であれば、有効なパリィかどうかを判定
	if (bIsPlayerParrying && !bIsMyAttackUnblockable)
	{
		bIsValidParry = UBluehorseFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	// GameplayAbility に渡すためのイベントデータを構築
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn(); // Attacker
	EventData.Target = HitActor;

	// 無敵状態であれば以降の処理は行わない
	if (bIsPlayerInvincible) return;

	// ===== ヒット結果に応じた分岐処理 =====


	if (bIsValidParry)
	{
		// パリィ成功時：プレイヤー側に成功パリィイベントを送信
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			BluehorseGameplayTags::Player_Event_SuccessfulParry,
			EventData
		);
	}
	else if (bIsPlayerHajiki)
	{
		// 弾き（Hajiki）成功時：専用イベントを送信
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			BluehorseGameplayTags::Player_Event_SuccessfulHajiki,
			EventData
		);
	}
	else if (bIsValidBlock)
	{
		// ブロック成功時：成功ブロックイベントを送信
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			BluehorseGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		// いずれにも該当しない場合は通常ヒットとして処理
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			BluehorseGameplayTags::Shared_Event_MeleeHit,
			EventData
		);

		// ヒットエフェクト・サウンドを Interface 経由で取得し再生
		if (IPawnHitEffectInterface* HitEffectInterface = Cast<IPawnHitEffectInterface>(HitActor))
		{
			// ヒットエフェクトの再生
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

			// ヒットサウンドの再生
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
