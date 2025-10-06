// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "BluehorseEnemyGameplayAbility.generated.h"

class ABluehorseEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseEnemyGameplayAbility : public UBluehorseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	ABluehorseEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);

private:
	TWeakObjectPtr<ABluehorseEnemyCharacter> CachedBluehorseEnemyCharacter;
};
