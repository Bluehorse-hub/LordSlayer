// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BluehorseTypes/BluehorseStructTypes.h"
#include "BluehorseAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FBluehorseHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Ability")
	void RemoveGtantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
