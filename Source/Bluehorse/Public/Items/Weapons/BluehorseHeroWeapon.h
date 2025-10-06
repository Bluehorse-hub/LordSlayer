// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/BluehorseWeaponBase.h"
#include "BluehorseTypes/BluehorseStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "BluehorseHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API ABluehorseHeroWeapon : public ABluehorseWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FBluehorseHeroWeaponData HeroWeaponData;

	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandle(const TArray<FGameplayAbilitySpecHandle>& InSpecHandle);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
	
};
