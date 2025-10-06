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

	// Turn����p�x�𓱏o
	float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw));

	bIsTurning = true;
	TurnInterpAlpha = 0.f;

	InitialYaw = CurrentRot.Yaw;
	TargetYaw = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw);
}

void UComputeAttackDirection::UpdateTurn(float DeltaTime)
{
	if (!bIsTurning) return;

	// Turn�ɂ������Ă��鎞�Ԃ��X�V���Ă���
	TurnInterpAlpha += DeltaTime / TurnDuration;

	// Turn�̐��ڂ͐��`�ōs��
	float InterpYaw = FMath::Lerp(InitialYaw, InitialYaw + TargetYaw, TurnInterpAlpha);

	FRotator NewRot = CachedHeroCharacter->GetActorRotation();
	NewRot.Yaw = InterpYaw;

	CachedHeroCharacter->SetActorRotation(NewRot);

	// FMath::Lerp()��Alpha�͂O�`�P�̒l���Ƃ�
	if (TurnInterpAlpha >= 1.f)
	{
		bIsTurning = false;
	}
}
