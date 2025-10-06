// Shungen All Rights Reserved.


#include "AI/BTService_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterSpeed = 5.f;
	Interval = 0.f;
	RandomDeviation = 0.f;

	// ����BTService���Q�Ƃ���Key��AActor�ɐ�������
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

// InTargetActorKey��R�Â��Ă���
void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

// �G�f�B�^��Ńm�[�h�̐����e�L�X�g��\�����邽�߂̊֐�
FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient rotation to %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	// ����BTService�𓮂���AIController�����ݏ������Ă���Pawn���擾
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn && TargetActor)
	{
		// �����̈ʒu->�^�[�Q�b�g�̈ʒu�ւ̉�]���Z�o�i������]�݂̂Ȃ̂�Pitch��Roll�͕s�v�j
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		LookAtRot.Pitch = 0.f;
		LookAtRot.Roll = 0.f;
		
		// ���݂̉�]����ڕW��]�֊��炩�ɕ⊮
		const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterSpeed);

		// ���ۂɉ�]������
		OwningPawn->SetActorRotation(TargetRot);
	}
}

