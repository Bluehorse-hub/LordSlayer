// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interact/BluehorseBaseInteract.h"
#include "BluehorsePortalInteract.generated.h"

/**
 * 
 */
class UBluehorseWidgetBase;

UCLASS()
class BLUEHORSE_API ABluehorsePortalInteract : public ABluehorseBaseInteract
{
	GENERATED_BODY()


public:
	ABluehorsePortalInteract();

public:
	// インタラクトの処理をオーバーライド
	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBluehorseWidgetBase> SelectWidget;
};
