// Shungen All Rights Reserved.


#include "AnimNotify/ToggleWeaponCollision.h"
#include "BluehorseFunctionLibrary.h"
#include "Components/Combat/PawnCombatComponent.h"

#include "BluehorseDebugHelper.h"

void UToggleWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	UPawnCombatComponent* CombatComponent = UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(Owner);

	CombatComponent->ToggleWeaponCollision(true, ToggleDamageType);

}

void UToggleWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	UPawnCombatComponent* CombatComponent = UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(Owner);

	CombatComponent->ToggleWeaponCollision(false, ToggleDamageType);
}

bool UToggleWeaponCollision::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}
