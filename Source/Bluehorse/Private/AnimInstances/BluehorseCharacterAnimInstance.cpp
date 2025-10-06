// Shungen All Rights Reserved.


#include "AnimInstances/BluehorseCharacterAnimInstance.h"
#include "Characters/BluehorseBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

// �A�j���̎g�p�҂���ړ���Ԃ��Q�Ƃ���֐�
void UBluehorseCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ABluehorseBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

// �A�j���[�V�����̍X�V�ɕK�v�Ȉړ��f�[�^�𖈃t���[���擾����֐�
void UBluehorseCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	// �X�g���C�t���Ă��鎞�̕������v�Z����
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
