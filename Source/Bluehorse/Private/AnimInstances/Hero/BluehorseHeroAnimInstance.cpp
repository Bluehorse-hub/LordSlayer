// Shungen All Rights Reserved.


#include "AnimInstances/Hero/BluehorseHeroAnimInstance.h"
#include "Characters/BluehorseHeroCharacter.h"

void UBluehorseHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningHeroCharacter = Cast<ABluehorseHeroCharacter>(OwningCharacter);
	}
}

void UBluehorseHeroAnimInstance::SetGroundSpeed(float InGroundSpeed)
{
	GroundSpeed = InGroundSpeed;
}

float UBluehorseHeroAnimInstance::GetGroundSpeed()
{
	return GroundSpeed;
}

bool UBluehorseHeroAnimInstance::GetbHasAcceleration()
{
	return bHasAcceleration;
}
