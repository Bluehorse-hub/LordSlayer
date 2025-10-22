// Shungen All Rights Reserved.


#include "AnimNotify/AnimNotifyState_SendEventTag.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotifyState_SendEventTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner || !Owner->HasActorBegunPlay())
	{
		// プレビュー再生時など、実ゲーム外では何もしない
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagOnBegin;
	Payload.EventMagnitude = 0.f;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagOnBegin, Payload);
}

void UAnimNotifyState_SendEventTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner || !Owner->HasActorBegunPlay())
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagOnEnd;
	Payload.EventMagnitude = 0.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagOnEnd, Payload);
}
