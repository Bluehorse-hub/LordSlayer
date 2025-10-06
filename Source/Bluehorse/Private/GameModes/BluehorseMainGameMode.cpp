// Shungen All Rights Reserved.


#include "GameModes/BluehorseMainGameMode.h"

void ABluehorseMainGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ABluehorseMainGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABluehorseMainGameMode::SetCurrentMainGameModeState(EBluehorseMainGameModeState InState)
{
	CurrentMainGameModeState = InState;

	OnMainGameModeStateChanged.Broadcast(CurrentMainGameModeState);
}
