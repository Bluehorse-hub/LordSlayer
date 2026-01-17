// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interact/BluehorseBaseInteract.h"
#include "BluehorseWeaponChangeInteract.generated.h"

class UBluehorseWidgetBase;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseWeaponChangeInteract : public ABluehorseBaseInteract
{
	GENERATED_BODY()

public:
	ABluehorseWeaponChangeInteract();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBluehorseWidgetBase> SelectWidget;
	
};
