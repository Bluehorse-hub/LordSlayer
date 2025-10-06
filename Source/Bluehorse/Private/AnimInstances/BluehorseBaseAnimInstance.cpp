// Shungen All Rights Reserved.


#include "AnimInstances/BluehorseBaseAnimInstance.h"
#include "BluehorseFunctionLibrary.h"

bool UBluehorseBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
    if (APawn* OwningPwan = TryGetPawnOwner())
    {
        return UBluehorseFunctionLibrary::NativeDoesActorHaveTag(OwningPwan, TagToCheck);
    }

    return false;
}
