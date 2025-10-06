// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/BluehorseCharacterAnimInstance.h"
#include "BluehorseHeroAnimInstance.generated.h"

class ABluehorseHeroCharacter;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseHeroAnimInstance : public UBluehorseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void SetGroundSpeed(float InGroundSpeed);

	UFUNCTION(BlueprintCallable)
	float GetGroundSpeed();

	UFUNCTION(BlueprintCallable)
	bool GetbHasAcceleration();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Refrences")
	ABluehorseHeroCharacter* OwningHeroCharacter;
};
