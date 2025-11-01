// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOE/BluehorseAOEBase.h"
#include "BluehorseAOEBlastBase.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseAOEBlastBase : public ABluehorseAOEBase
{
	GENERATED_BODY()

protected:
	virtual void SpawnAOESphere_Implementation() override;
	virtual void OnAOEOverlapActor_Implementation(AActor* OtherActor) override;
};
