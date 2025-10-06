// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/BluehorseBaseAnimInstance.h"
#include "BluehorseTypes/BluehorseEnumTypes.h"
#include "BluehorseHeroLinkedAnimLayer.generated.h"

class UBluehorseHeroAnimInstance;
class ABluehorseHeroCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseHeroLinkedAnimLayer : public UBluehorseBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void PreUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UBluehorseHeroAnimInstance* GetHeroAnimInstance() const;

	UPROPERTY(EditDefaultsOnly)
	float DefaultMaxWalkSpeed = 500.f;;

	UPROPERTY(EditDefaultsOnly)
	float BlockingMaxWalkSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly)
	float TargetLockingMaxWalkSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly)
	float BlockingAndTargetLockingMaxWalkSpeed = 150.f;
	
	UPROPERTY(BlueprintReadOnly)
	ELocomotionType LocomotionType;

	ABluehorseHeroCharacter* OwningHeroCharacter;

	UPROPERTY()
	UCharacterMovementComponent* MovementComp;
};
