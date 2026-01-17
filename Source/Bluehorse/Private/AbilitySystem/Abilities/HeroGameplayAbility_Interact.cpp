// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_Interact.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/InteractableInterface.h"

// 近距離のインタラクト対象を前方ラインテレースで探索し
// InteractableInterface を実装していれば Interact を実行する即時型 Ability
void UHeroGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Ability の実行主体（プレイヤー）を取得
	ABluehorseHeroCharacter* Avator = GetHeroCharacterFromActorInfo();

	if (!Avator) return;

	// ローカル動作前提でも、World が取れないケース（破棄中など）を避けるためチェック
	UWorld* World = Avator->GetWorld();
	if (!World) return;

	// プレイヤーの前方へ一定距離ラインテレースしてインタラクト対象を探す
	FVector Start = Avator->GetActorLocation();
	FVector End = Start + Avator->GetActorForwardVector() * LineTraceLength;

	FHitResult HitResult;

	// ここでは ObjectTypes 指定で対象を絞る
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));

	// 自分自身には当てない
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Avator);

	// デバッグ描画は ForDuration（検証用)
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		World,
		Start,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (bHit)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			// インタラクト可能かどうかは Interface 実装で判定
			// 個別クラスへの依存を避けることで、対象追加が容易になる
			if (HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				// BlueprintNativeEvent / BlueprintImplementableEvent 想定：Execute_Interact で呼び出す
				// 引数に Avatar を渡して「誰がインタラクトしたか」を対象側で利用できるようにする
				IInteractableInterface::Execute_Interact(HitActor, Avator);
			}
		}
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 即時型なのでこのフレームで終了する
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UHeroGameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 現状は後片付けなし
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
