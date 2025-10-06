// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/BluehorseBaseAnimInstance.h"
#include "BluehorseCharacterAnimInstance.generated.h"

class ABluehorseBaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseCharacterAnimInstance : public UBluehorseBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	UPROPERTY()
	ABluehorseBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
};
