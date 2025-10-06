// Shungen All Rights Reserved.


#include "AnimInstances/BluehorseCharacterAnimInstance.h"
#include "Characters/BluehorseBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

// アニメの使用者から移動状態を参照する関数
void UBluehorseCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ABluehorseBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

// アニメーションの更新に必要な移動データを毎フレーム取得する関数
void UBluehorseCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	// ストレイフしている時の方向を計算する
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
