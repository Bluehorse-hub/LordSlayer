// Shungen All Rights Reserved.


#include "Interact/BluehorseSpawnBossInteract.h"
#include "Kismet/GameplayStatics.h"

ABluehorseSpawnBossInteract::ABluehorseSpawnBossInteract()
{
	Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABluehorseSpawnBossInteract::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->SetVisibility(false, true);
		Mesh->SetHiddenInGame(true);
	}
}

void ABluehorseSpawnBossInteract::SpawnBoss(AActor* InSpawnBoss, FTransform InSpawnBossTransform)
{
	if (IsSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnBoss] Already spawn"));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedBoss = GetWorld()->SpawnActor<AActor>(
		BossClass,
		BossTransform,
		Params
	);

	if (SpawnedBoss)
	{
		IsSpawn = true;
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnBoss] Spawn FAILED"));
		return;
	}
}
