// Shungen All Rights Reserved.


#include "AnimNotify/AnimNotify_SendEventTagToOwner.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

// アニメーション再生中の任意フレームで GameplayEvent（GameplayTag）を Owner に送信する AnimNotify
void UAnimNotify_SendEventTagToOwner::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// MeshComp が無効な場合は何もしない（安全対策）
	if (!MeshComp) return;

	// このアニメを再生している Actor
	AActor* Owner = MeshComp->GetOwner();

	// エディタプレビューや BeginPlay 前ではイベントを送らない
	// （AbilitySystem が未初期化のケースを防ぐ）
	if (!Owner || !Owner->HasActorBegunPlay())
	{
		return;
	}

	// Owner から AbilitySystemComponent を取得
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	// ASC を持たない Actor の場合はイベント送信不可
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SendEventTagToOwner] %s has no ASC, skipping event: %s"),
			*Owner->GetName(), *EventTagToSend.ToString());
		return;
	}

	// 送信する EventTag が未設定なら警告を出して終了
	if (!EventTagToSend.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SendEventTagToOwner] EventTag is invalid for %s"), *Owner->GetName());
		return;
	}

	// GameplayEvent に渡すペイロードを構築
	// 必要に応じて TargetData / OptionalObject なども拡張可能

	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagToSend;
	Payload.EventMagnitude = 0.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagToSend, Payload);

	UE_LOG(LogTemp, Log, TEXT("[SendEventTagToOwner] Sent EventTag %s to %s"), *EventTagToSend.ToString(), *Owner->GetName());
}
