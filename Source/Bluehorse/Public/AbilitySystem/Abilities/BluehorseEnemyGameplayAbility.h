// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "BluehorseEnemyGameplayAbility.generated.h"

class ABluehorseEnemyCharacter;
class UEnemyCombatComponent;
class ABluehorseAIController;
class UBlackboardComponent;
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
	ABluehorseAIController* GetEnemyAIControllerFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);

	UFUNCTION(BlueprintCallable, Category = "Ability|Blackboard")
	UObject* GetBlackboardValueAsObject(FName KeyName) const;

private:
	TWeakObjectPtr<ABluehorseEnemyCharacter> CachedBluehorseEnemyCharacter;
	mutable TWeakObjectPtr<ABluehorseAIController> CachedBluehorseAIController;
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboardComponent;
};
