// Shungen All Rights Reserved.


#include "AI/BTTask_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate to Face Target Actor");
	AnglePrecision = 10.f;
	RotationInterSpeed = 5.f;

	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

// InTargetActorKey��R�Â��Ă���
void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

// ����Task��p�̎��s���������̈�T�C�Y��Ԃ�
uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

// �G�f�B�^��Ńm�[�h�̐����e�L�X�g��\�����邽�߂̊֐�
FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision %sf is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	// ���̃^�X�N�����s���Ă���AI��Pawn���擾
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	// ���̃^�X�N���s�C���X�^���X��p�̃������̈���擾
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);

	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	// ���������̊m�F
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	// ���ɏ\����]���Ă��邩�m�F
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	// OwningPawn/TargetActor�̂ǂ��炩�����Ȃ���Ύ��s�ő��I��
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// ���ɏ\����]���Ă��邩�m�F
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	else
	{
		// ������]�݂̂��l����
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		LookAtRot.Pitch = 0.f;
		LookAtRot.Roll = 0.f;

		// ��]�̕⊮
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterSpeed);

		// ���ۂɉ�]
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal2D();
	const float TargetYaw = OwnerToTargetNormalized.Rotation().Yaw;

	const float CurrentYaw = QueryPawn->GetActorRotation().Yaw;
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

	return FMath::Abs(DeltaYaw) <= AnglePrecision;

	/*const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);

	return AngleDiff <= AnglePrecision;*/
}