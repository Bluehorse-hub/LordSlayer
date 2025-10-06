// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/BluehorseBaseGameMode.h"
#include "BluehorseMainGameMode.generated.h"

UENUM(BlueprintType)
enum class EBluehorseMainGameModeState : uint8
{
	PlayerWon,
	PlayerDied
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainGameModeStateChangedDelegate, EBluehorseMainGameModeState, CurrentState);

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseMainGameMode : public ABluehorseBaseGameMode
{
	GENERATED_BODY()
	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	EBluehorseMainGameModeState CurrentMainGameModeState;

	void SetCurrentMainGameModeState(EBluehorseMainGameModeState InState);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMainGameModeStateChangedDelegate OnMainGameModeStateChanged;
};
