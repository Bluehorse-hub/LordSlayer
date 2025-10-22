// Shungen All Rights Reserved.


#include "AnimNotify/AnimNotify_SendEventTagToOwner.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAnimNotify_SendEventTagToOwner::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner || !Owner->HasActorBegunPlay())
	{
		// エディタプレビューなどではスキップ
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SendEventTagToOwner] %s has no ASC, skipping event: %s"),
			*Owner->GetName(), *EventTagToSend.ToString());
		return;
	}

	if (!EventTagToSend.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SendEventTagToOwner] EventTag is invalid for %s"), *Owner->GetName());
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	Payload.EventTag = EventTagToSend;
	Payload.EventMagnitude = 0.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTagToSend, Payload);

	UE_LOG(LogTemp, Log, TEXT("[SendEventTagToOwner] Sent EventTag %s to %s"), *EventTagToSend.ToString(), *Owner->GetName());
}
