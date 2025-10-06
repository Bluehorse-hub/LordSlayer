// Shungen All Rights Reserved.


#include "Interact/BluehorseBaseInteract.h"

#include "BluehorseDebugHelper.h"

// Sets default values
ABluehorseBaseInteract::ABluehorseBaseInteract()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	RootComponent = Mesh;
}

void ABluehorseBaseInteract::Interact_Implementation(AActor* Interactor)
{
	FString Message = FString::Printf(TEXT("%s interacted with %s"), *GetName(), *Interactor->GetName());
	Debug::Print(Message, FColor::Blue, 4);
}
