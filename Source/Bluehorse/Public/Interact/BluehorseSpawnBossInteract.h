// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interact/BluehorseBaseInteract.h"
#include "BluehorseSpawnBossInteract.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseSpawnBossInteract : public ABluehorseBaseInteract
{
	GENERATED_BODY()

public:
	ABluehorseSpawnBossInteract();

	void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnBoss();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss")
	TSubclassOf<AActor> BossClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss")
	FTransform BossTransform;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Boss")
	bool IsSpawn = false;
};
