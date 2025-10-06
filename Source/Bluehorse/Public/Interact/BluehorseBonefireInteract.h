// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interact/BluehorseBaseInteract.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BluehorseBonefireInteract.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseBonefireInteract : public ABluehorseBaseInteract
{
	GENERATED_BODY()

public:
    ABluehorseBonefireInteract();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
    UParticleSystemComponent* FireParticleComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonfire")
    UParticleSystem* FireParticleSystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
    UAudioComponent* FireAudioComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonfire")
    USoundBase* FireLoopSound;

    virtual void BeginPlay() override;
	
};
