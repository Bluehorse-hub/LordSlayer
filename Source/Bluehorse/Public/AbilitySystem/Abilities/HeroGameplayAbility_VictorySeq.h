// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "HeroGameplayAbility_VictorySeq.generated.h"

class ACineCameraActor;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UHeroGameplayAbility_VictorySeq : public UBluehorseHeroGameplayAbility
{
	GENERATED_BODY()

private:

	UFUNCTION(BlueprintCallable)
	void SpawnVictoryCamera();

	UFUNCTION(BlueprintCallable)
	void SettingVictoryCamera(float CurrentFocalLength, float CurrentAperture, AActor* ActorToTrack);

	/** 演出専用 CineCameraActor クラス */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACineCameraActor> VictoryCameraClass;

	/** 生成された実体 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	ACineCameraActor* VictoryCameraActor;
};
