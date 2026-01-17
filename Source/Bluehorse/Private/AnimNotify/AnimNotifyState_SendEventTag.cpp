// Shungen All Rights Reserved.


#include "AnimNotify/AnimNotifyState_SendEventTag.h"
#include "AbilitySystemBlueprintLibrary.h"

// AnimNotifyState の開始 / 終了タイミングで GameplayEvent（GameplayTag）を送信するクラス

void UAnimNotifyState_SendEventTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// アニメを再生している Actor
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	// エディタプレビューや BeginPlay 前ではイベントを送らない
	if (!Owner || !Owner->HasActorBegunPlay())
	{
		return;
	}

	// Begin 用 GameplayEvent のペイロードを構築
	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagOnBegin;
	Payload.EventMagnitude = 0.f;
	
	// 状態開始イベントを送信
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagOnBegin, Payload);
}

// AnimNotifyState の終了時に呼ばれる
void UAnimNotifyState_SendEventTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner || !Owner->HasActorBegunPlay())
	{
		return;
	}

	// End 用 GameplayEvent のペイロードを構築
	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagOnEnd;
	Payload.EventMagnitude = 0.f;

	// 状態終了イベントを送信
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagOnEnd, Payload);
}
