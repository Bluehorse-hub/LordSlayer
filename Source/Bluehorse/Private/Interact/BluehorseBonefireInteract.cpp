// Shungen All Rights Reserved.


#include "Interact/BluehorseBonefireInteract.h"

#include "BluehorseDebugHelper.h"

ABluehorseBonefireInteract::ABluehorseBonefireInteract()
{

    FireParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticleComponent"));
    FireParticleComponent->SetupAttachment(GetRootComponent());
    FireParticleComponent->bAutoActivate = true;

    FireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioComponent"));
    FireAudioComponent->SetupAttachment(GetRootComponent());
    FireAudioComponent->bAutoActivate = false;
}

void ABluehorseBonefireInteract::BeginPlay()
{
    Super::BeginPlay();

    if (FireParticleSystem)
    {
        //Debug::Print(TEXT("FireParticleSystem is exist"));
        FireParticleComponent->SetTemplate(FireParticleSystem);
        FireParticleComponent->Activate(true);
    }

    if (FireLoopSound)
    {
        FireAudioComponent->SetSound(FireLoopSound);
        FireAudioComponent->Play();
    }
}
