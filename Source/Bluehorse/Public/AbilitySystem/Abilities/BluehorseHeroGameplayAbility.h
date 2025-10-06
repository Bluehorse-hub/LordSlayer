// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "BluehorseHeroGameplayAbility.generated.h"

class ABluehorseHeroCharacter;
class ABluehorseHeroController;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseHeroGameplayAbility : public UBluehorseGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	ABluehorseHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	ABluehorseHeroController* GetHeroControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag);

private:
	TWeakObjectPtr<ABluehorseHeroCharacter> CachedBluehorseHeroCharacter;
	TWeakObjectPtr<ABluehorseHeroController> CachedBluehorseHeroController;
};
