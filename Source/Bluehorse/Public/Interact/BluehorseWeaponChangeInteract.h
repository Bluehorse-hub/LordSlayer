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
	// インタラクトの処理をオーバーライド
	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBluehorseWidgetBase> SelectWidget;
	
};
