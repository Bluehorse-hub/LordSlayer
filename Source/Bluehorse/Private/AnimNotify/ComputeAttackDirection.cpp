// Shungen All Rights Reserved.


#include "AnimNotify/ComputeAttackDirection.h"
#include "Characters/BluehorseHeroCharacter.h"

void UComputeAttackDirection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	CachedHeroCharacter = Cast<ABluehorseHeroCharacter>(MeshComp->GetOwner());
}

void UComputeAttackDirection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	TryStartTurn();
	UpdateTurn(FrameDeltaTime);
}

void UComputeAttackDirection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	bIsTurning = false;
	CachedHeroCharacter.Reset();
}

bool UComputeAttackDirection::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}

void UComputeAttackDirection::TryStartTurn()
{
	if (bIsTurning) return;

	FVector InputVector = CachedHeroCharacter->GetLastMovementInputVector();
	if (InputVector.IsNearlyZero()) return;

	FRotator CurrentRot = CachedHeroCharacter->GetActorRotation();
	FRotator InputRot = InputVector.Rotation();

	// Turn‚·‚éŠp“x‚ð“±o
	float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw));

	bIsTurning = true;
	TurnInterpAlpha = 0.f;

	InitialYaw = CurrentRot.Yaw;
	TargetYaw = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw);
}

void UComputeAttackDirection::UpdateTurn(float DeltaTime)
{
	if (!bIsTurning) return;

	// Turn‚É‚©‚©‚Á‚Ä‚¢‚éŽžŠÔ‚ðXV‚µ‚Ä‚¢‚­
	TurnInterpAlpha += DeltaTime / TurnDuration;

	// Turn‚Ì„ˆÚ‚ÍüŒ`‚Ås‚¤
	float InterpYaw = FMath::Lerp(InitialYaw, InitialYaw + TargetYaw, TurnInterpAlpha);

	FRotator NewRot = CachedHeroCharacter->GetActorRotation();
	NewRot.Yaw = InterpYaw;

	CachedHeroCharacter->SetActorRotation(NewRot);

	// FMath::Lerp()‚ÌAlpha‚Í‚O`‚P‚Ì’l‚ð‚Æ‚é
	if (TurnInterpAlpha >= 1.f)
	{
		bIsTurning = false;
	}
}
