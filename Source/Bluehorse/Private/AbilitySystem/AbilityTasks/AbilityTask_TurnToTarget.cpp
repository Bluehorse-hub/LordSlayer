// Shungen All Rights Reserved.


#include "AbilitySystem/AbilityTasks/AbilityTask_TurnToTarget.h"
#include "BluehorseFunctionLibrary.h"

//  ���t���[��Tick���K�v�ȃ^�X�N�ł��邽�߁AbTickingTask = true�ɐݒ�B
UAbilityTask_TurnToTarget::UAbilityTask_TurnToTarget()
{
	bTickingTask = true;
}

UAbilityTask_TurnToTarget* UAbilityTask_TurnToTarget::TurnToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, float InterpSpeed, FGameplayTag EndTag)
{
	UAbilityTask_TurnToTarget* Node = NewAbilityTask<UAbilityTask_TurnToTarget>(OwningAbility);

	// �p�����[�^�ݒ�
	Node->Target = TargetActor;
	Node->RotationSpeed = InterpSpeed;
	Node->EndCheckTag = EndTag;
	return Node;
}

// ���t���[���^�[�Q�b�g�������v�Z���A�w�葬�x�ŃA�o�^�[����]������B
// �����EndTag���A�o�^�[�ɕt�^���ꂽ�ꍇ�͎����I�ɏI���B
void UAbilityTask_TurnToTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// Broadcast�ł��Ȃ� or �^�[�Q�b�g�������Ȃ�I��
	if (!ShouldBroadcastAbilityTaskDelegates() || !Target.IsValid())
	{
		EndTask();
		return;
	}

	// Ability�̏��L�A�N�^�[�i���ۂɉ�]������L�����j���擾
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	// �w�肳�ꂽEndTag���t�^����Ă���΃^�X�N�I��
	if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Avatar, EndCheckTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("[TurnToTarget] EndTag detected ? Task Ending."));
		EndTask();
		return;
	}

	// �^�[�Q�b�g�����ւ̉�]���v�Z�i�����ʂ̂݁j
	FVector ToTarget = Target->GetActorLocation() - Avatar->GetActorLocation();
	ToTarget.Z = 0.f;

	if (ToTarget.IsNearlyZero()) return;

	// ���݊p�x�ƖڕW�p�x���擾
	FRotator CurrentRot = Avatar->GetActorRotation();
	FRotator TargetRot = ToTarget.Rotation();

	// RInterpTo�ŃX���[�Y�ɕ��
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);

	Avatar->SetActorRotation(NewRot);
}

// �^�X�N�I������
// �I�����Ƀ^�[�Q�b�g�Q�Ƃ��N���A���Ĉ��S�ɔj���B
void UAbilityTask_TurnToTarget::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	Target.Reset();  // WeakPtr�̃N���A
}


