// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_Interact.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/InteractableInterface.h"

void UHeroGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ABluehorseHeroCharacter* Avator = GetHeroCharacterFromActorInfo();

	if (!Avator) return;

	UWorld* World = Avator->GetWorld();

	if (!World) return;

	FVector Start = Avator->GetActorLocation();
	FVector End = Start + Avator->GetActorForwardVector() * LineTraceLength;

	FHitResult HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Avator);

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
			if (HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				IInteractableInterface::Execute_Interact(HitActor, Avator);
			}
		}
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UHeroGameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
