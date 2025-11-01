// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"

// Abilityが「付与された（Given）」際に呼ばれる
//（GASがキャラクターにAbilityを登録したタイミング）
void UBluehorseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// アクティベーションポリシーが「付与時に即発動（OnGiven）」なら自動発動
	if (AbilityActivationPolicy == EBluehorseAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			// AbilitySystemComponentを通してAbilityを起動
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

// Abilityが終了したときに呼ばれる
void UBluehorseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//「付与時に即発動するタイプ」のAbilityなら、発動が終わったら自動で削除
	if (AbilityActivationPolicy == EBluehorseAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

// Avatar（Pawn）からCombatComponentを取得
//（近接攻撃や戦闘関連の共通機能を呼び出すため）
UPawnCombatComponent* UBluehorseGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass< UPawnCombatComponent>();
}

// AbilitySystemComponentを自作クラス型で取得
//（自作の拡張ASC機能を使うため）
UBluehorseAbilitySystemComponent* UBluehorseGameplayAbility::GetBluehorseAbilitySystemComponentFromActorInfo() const
{
	return Cast<UBluehorseAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

// C++ネイティブ側で指定ターゲットへEffectを適用
FActiveGameplayEffectHandle UBluehorseGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	// ターゲットのASCを取得
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// 有効性チェック（nullptrや無効Specを防ぐ）
	check(TargetASC && InSpecHandle.IsValid());

	// 自分のASCからターゲットへEffectを適用
	return GetBluehorseAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

// BlueprintCallable版：BPから呼ぶためのラッパ
// 成功／失敗を返すようにしている
FActiveGameplayEffectHandle UBluehorseGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EBluehorseSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);

	// 成否をEnumで返す
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EBluehorseSuccessType::Successful : EBluehorseSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

// 複数HitResultに対してEffectを適用する（範囲攻撃など）
void UBluehorseGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
	// 何も当たっていないなら処理終了
	if (InHitResults.IsEmpty())
	{
		return;
	}

	// 自分自身のPawnを取得
	APawn* OwninPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	// 全Hit結果をループ
	for (const FHitResult& Hit : InHitResults)
	{
		// ヒット対象がPawnである場合のみ処理
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			// 敵対関係チェック（味方への誤爆を防止）
			if (UBluehorseFunctionLibrary::IsTargetPawnHostile(OwninPawn, HitPawn))
			{
				// Effectを適用
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				// 成功したらHitReactイベントを発火
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwninPawn;
					Data.Target = HitPawn;

					// Hit反応用のGameplayEventを送信
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						BluehorseGameplayTags::Shared_Event_HitReact,
						Data
					);
				}
			}
		}
	}
}
