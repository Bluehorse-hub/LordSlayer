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
	// �C���^���N�g�̏������I�[�o�[���C�h
	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBluehorseWidgetBase> SelectWidget;
};
