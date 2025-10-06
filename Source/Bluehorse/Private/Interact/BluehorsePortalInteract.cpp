// Shungen All Rights Reserved.


#include "Interact/BluehorsePortalInteract.h"
#include "Widgets/BluehorseWidgetBase.h"

#include "BluehorseDebugHelper.h"

ABluehorsePortalInteract::ABluehorsePortalInteract()
{
}

void ABluehorsePortalInteract::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	Debug::Print(TEXT("Activated the portal!"), FColor::Blue, 3);
}
